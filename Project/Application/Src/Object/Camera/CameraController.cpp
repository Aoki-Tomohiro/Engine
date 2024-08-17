#include "CameraController.h"
#include "Application/Src/Object/Camera/States/CameraStateFollow.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Weapon/Weapon.h"

void CameraController::Initialize()
{
	//カメラの初期化
	camera_.Initialize();
	camera_.rotationType_ = RotationType::Quaternion;

	//Stateの初期化
	ChangeState(new CameraStateFollow());
}

void CameraController::Update()
{
	//追従対象が存在する場合、追従座標を補間する
	if (target_)
	{
		Vector3 targetPosition = {
			target_->matWorld_.m[3][0],
			target_->matWorld_.m[3][1],
			target_->matWorld_.m[3][2],
		};
		interTarget_ = Mathf::Lerp(interTarget_, targetPosition, targetInterpolationSpeed_);
	}

	//デバッグのフラグが立っていなくアニメーションの再生がされていない場合
	if (!cameraPathManager_->GetIsEditModeEnabled() && !cameraPathManager_->GetIsPlayAnimation())
	{
		//状態の更新
		state_->Update();
	}
	else
	{
		//アニメーションが再生されている場合
		if (cameraPathManager_->GetIsPlayAnimation())
		{
			//ターゲットがいる場合
			if (target_)
			{
				//アニメーションを進める
				CameraKeyFrame cameraKeyFrame = cameraPathManager_->GetCurrentKeyFrame();
				//キーフレームの情報をもとに設定
				destinationOffset_ = cameraKeyFrame.position;
				const Player* player = GameObjectManager::GetInstance()->GetConstGameObject<Player>("");
				Quaternion quaternion = player->GetDestinationQuaternion() * Mathf::Conjugate(Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, std::numbers::pi_v<float>));
				destinationQuaternion_ = quaternion * cameraKeyFrame.rotation;
				camera_.fov_ = cameraKeyFrame.fov * std::numbers::pi_v<float> / 180.0f;
			}
		}
		else
		{
			//ターゲットがいる場合
			if (target_)
			{
				//編集中のキーフレームを取得
				CameraKeyFrame cameraKeyFrame = cameraPathManager_->GetNewKeyFrame();
				//キーフレームの情報をもとに設定
				destinationOffset_ = cameraKeyFrame.position;
				const Player* player = GameObjectManager::GetInstance()->GetConstGameObject<Player>("");
				Quaternion quaternion = player->GetDestinationQuaternion() * Mathf::Conjugate(Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, std::numbers::pi_v<float>));
				destinationQuaternion_ = quaternion * cameraKeyFrame.rotation;
				camera_.fov_ = cameraKeyFrame.fov * std::numbers::pi_v<float> / 180.0f;
			}
		}

		//追従対象からのオフセットを取得してカメラ座標を計算する
		Vector3 offset = Offset();
		SetPosition(GetInterTarget() + offset);
	}

	//オフセット値を補間する
	offset_ = Mathf::Lerp(offset_, destinationOffset_, offsetInterpolationSpeed_);

	//回転処理
	destinationQuaternion_ = Mathf::Normalize(destinationQuaternion_);
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

const Vector3 CameraController::Offset() const
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

void CameraController::UpdateCameraShake()
{
	//プレイヤーの攻撃が当たっていた時
	GameObjectManager* gameObjectManager = GameObjectManager::GetInstance();
	if (gameObjectManager->GetConstGameObject<Weapon>("PlayerWeapon")->GetIsHit())
	{
		//カメラシェイクを有効にする
		cameraShakeSettings_.isShaking = true;
		//カメラシェイクのタイマーをリセット
		cameraShakeSettings_.timer = 0.0f;
		//プレイヤーを取得
		const Player* player = GameObjectManager::GetInstance()->GetConstGameObject<Player>("");
		//カメラシェイクの強度とシェイク間隔を決める
		cameraShakeSettings_.intensity = cameraShakeParameters_.baseIntensity;
		cameraShakeSettings_.duration = cameraShakeParameters_.baseDuration;
		if (player->GetActionFlag(Player::ActionFlag::kFallingAttack))
		{
			cameraShakeSettings_.intensity = cameraShakeParameters_.justDodgeIntensity;
			cameraShakeSettings_.duration = cameraShakeParameters_.justDodgeDuration;
		}
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

		//現在のシェイク強度
		Vector3 currentIntensity = cameraShakeSettings_.intensity * (1.0f - easedProgress);

		//現在のシェイク間隔
		float duration = cameraShakeSettings_.duration;

		//現在の強度を基にランダムな揺らし幅を生成
		Vector3 shakeOffset = {
			RandomGenerator::GetRandomFloat(-currentIntensity.x,currentIntensity.x),
			RandomGenerator::GetRandomFloat(-currentIntensity.y,currentIntensity.y),
			RandomGenerator::GetRandomFloat(-currentIntensity.z,currentIntensity.z),
		};

		//カメラの座標にランダムな揺らし幅を加算
		camera_.translation_ += shakeOffset;

		//シェイクタイマーが指定の時間を超えたらカメラシェイクを無効化する
		if (cameraShakeSettings_.timer > duration)
		{
			cameraShakeSettings_.isShaking = false;
		}
	}
}