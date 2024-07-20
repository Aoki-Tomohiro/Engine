#include "CameraController.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Utilities/GameTimer.h"
#include "Engine/Utilities/RandomGenerator.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/CameraController/States/CameraStateFollow.h"

void CameraController::Initialize()
{
	//カメラの初期化
	camera_.Initialize();
	camera_.rotationType_ = RotationType::Quaternion;

	//Stateの初期化
	ChangeState(new CameraStateFollow());
};

void CameraController::Update()
{
	//追従対象が存在する場合、追従座標を補間する
	if (target_)
	{
		interTarget_ = Mathf::Lerp(interTarget_, target_->translation_, targetInterpolationSpeed_);
	}

	//オフセット値を補間する
	offset_ = Mathf::Lerp(offset_, destinationOffset_, offsetInterpolationSpeed_);

	//状態の更新
	state_->Update();

	//回転処理
	camera_.quaternion_ = Mathf::Normalize(Mathf::Slerp(camera_.quaternion_, destinationQuaternion_, quaternionInterpolationSpeed_));

	//カメラシェイクの処理
	UpdateCameraShake();

	//カメラの更新
	camera_.UpdateMatrix();
}

void CameraController::ChangeState(ICameraState* state)
{
	state->SetCameraController(this);
	state->Initialize();
	state_.reset(state);
}

void CameraController::UpdateCameraShake()
{
	//プレイヤーの攻撃が当たっていた時
	GameObjectManager* gameObjectManager = GameObjectManager::GetInstance();
	if (gameObjectManager->GetGameObject<Weapon>()->GetIsHit())
	{
		//カメラシェイクを有効にする
		cameraShakeSettings_.isShaking = true;
		//カメラシェイクのタイマーをリセット
		cameraShakeSettings_.timer = 0.0f;
	}

	//カメラシェイクが有効な場合
	if (cameraShakeSettings_.isShaking)
	{
		//シェイクタイマーを進める
		cameraShakeSettings_.timer += GameTimer::GetDeltaTime();

		//現在の進行状況を計算
		float currentTime = cameraShakeSettings_.timer / cameraShakeSettings_.duration;

		//イージング関数を適用して、シェイクの強さを滑らかに減衰させる
		float easedProgress = Mathf::EaseOutExpo(currentTime);

		//現在のシェイク強度を計算
		Vector3 currentIntensity = {
			cameraShakeSettings_.intensity.x * (1.0f - easedProgress),
			cameraShakeSettings_.intensity.y * (1.0f - easedProgress),
			cameraShakeSettings_.intensity.z * (1.0f - easedProgress),
		};

		//現在の強度を基にランダムな揺らし幅を生成
		Vector3 shakeOffset = {
			RandomGenerator::GetRandomFloat(-currentIntensity.x,currentIntensity.x),
			RandomGenerator::GetRandomFloat(-currentIntensity.y,currentIntensity.y),
			RandomGenerator::GetRandomFloat(-currentIntensity.z,currentIntensity.z),
		};

		//カメラの座標にランダムな揺らし幅を加算
		camera_.translation_ += shakeOffset;

		//シェイクタイマーが指定の時間を超えたらカメラシェイクを無効化する
		if (cameraShakeSettings_.timer > cameraShakeSettings_.duration)
		{
			cameraShakeSettings_.isShaking = false;
		}
	}
}

Vector3 CameraController::Offset()
{
	//追従対象からのオフセット
	Vector3 offset = offset_;
	//回転行列の合成
	Matrix4x4 rotateMatrix = Mathf::MakeRotateMatrix(camera_.quaternion_);
	//オフセットをカメラの回転に合わせて回転させる
	offset = Mathf::TransformNormal(offset, rotateMatrix);
	//オフセット値を返す
	return offset;
}