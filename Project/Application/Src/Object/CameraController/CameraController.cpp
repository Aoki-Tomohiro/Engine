#include "CameraController.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GameTimer.h"
#include "Engine/Utilities/RandomGenerator.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include <numbers>

void CameraController::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//カメラの初期化
	camera_.Initialize();

	//環境変数の設定
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "CameraController";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "ShakeDuration", cameraShakeSettings_.duration);
	globalVariables->AddItem(groupName, "ShakeIntensity", cameraShakeSettings_.intensity);
};

void CameraController::Update()
{
	//追従対象が存在する場合、追従座標を補間する
	if (target_)
	{
		interTarget_ = Mathf::Lerp(interTarget_, target_->translation_, 0.2f);
	}

	//オフセット値を補間する
	offset_ = Mathf::Lerp(offset_, destinationOffset_, 0.1f);
	//追従対象からのオフセットを取得してカメラ座標を計算する
	Vector3 offset = Offset();
	camera_.translation_ = interTarget_ + offset;

	//ロックオン中の場合の処理
	if (lockOn_ && lockOn_->ExistTarget()) 
	{
		Vector3 lockOnPosition = lockOn_->GetTargetPosition();
		Vector3 sub = lockOnPosition - target_->translation_;
		sub.y = 0.0f;

		//Y軸周りの角度を計算
		if (sub.z != 0.0)
		{
			destinationAngleY_ = std::asin(sub.x / std::sqrt(sub.x * sub.x + sub.z * sub.z));

			if (sub.z < 0.0)
			{
				destinationAngleY_ = (sub.x >= 0.0) ? std::numbers::pi_v<float> -destinationAngleY_ : -std::numbers::pi_v<float> -destinationAngleY_;
			}
		}
		else 
		{
			destinationAngleY_ = (sub.x >= 0.0) ? std::numbers::pi_v<float> / 2.0f : -std::numbers::pi_v<float> / 2.0f;
		}

		destinationAngleX_ = 0.2f;//X軸角度の設定
	}
	//ロックオンしていない場合はカメラ操作の処理を行う
	else 
	{
		const float threshold = 0.7f;//スティックの押し込みしきい値
		bool isRotation = false;//回転フラグ

		//右スティックの入力を取得
		Vector3 rotation = {
			input_->GetRightStickY(),
			input_->GetRightStickX(),
			0.0f
		};

		//スティックの押し込みが遊び範囲を超えていたら回転フラグをtrueにする
		if (Mathf::Length(rotation) > threshold)
		{
			isRotation = true;
		}

		if (isRotation)
		{
			const float kRotSpeedX = 0.02f;//X軸回転速度
			const float kRotSpeedY = 0.04f;//Y軸回転速度

			//回転を適用
			destinationAngleX_ -= rotation.x * kRotSpeedX;
			destinationAngleY_ += rotation.y * kRotSpeedY;
		}
	}

	//カメラシェイクの処理
	UpdateCameraShake();

	//回転限界角度
	const float kRotateMin = -0.14f;
	const float kRotateMax = 0.4f;
	destinationAngleX_ = std::min<float>(destinationAngleX_, kRotateMax);
	destinationAngleX_ = std::max<float>(destinationAngleX_, kRotateMin);

	//カメラのに回転を適用
	camera_.rotation_.x = Mathf::LerpShortAngle(camera_.rotation_.x, destinationAngleX_, 0.1f);
	camera_.rotation_.y = Mathf::LerpShortAngle(camera_.rotation_.y, destinationAngleY_, 0.1f);

	//カメラリセットの処理
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_THUMB))
	{
		Reset();
	}

	//カメラの更新
	camera_.UpdateMatrix();

	ImGui::Begin("CameraController");
	ImGui::End();
};

void CameraController::UpdateCameraShake()
{
	//プレイヤーの攻撃が当たっていた時
	GameObjectManager* gameObjectManager = GameObjectManager::GetInstance();
	if (gameObjectManager->GetGameObject<Weapon>()->GetIsHit())
	{
		//プレイヤーを取得
		Player* player = gameObjectManager->GetGameObject<Player>();
		//プレイヤーの攻撃が最後のコンボの時
		if (player->GetComboIndex() == player->GetComboNum() - 1)
		{
			//カメラシェイクを有効にする
			cameraShakeSettings_.isShaking = true;
			cameraShakeSettings_.offset = camera_.translation_;
		}
	}

	//カメラシェイクが有効な場合
	if (cameraShakeSettings_.isShaking)
	{
		//シェイクタイマーを進める
		const float kShakeDeltaTime = 1.0f / 60.0f;
		cameraShakeSettings_.timer += kShakeDeltaTime;

		//揺らし幅をランダムで決める
		Vector3 randomValue = {
			RandomGenerator::GetRandomFloat(-cameraShakeSettings_.intensity.x,cameraShakeSettings_.intensity.x),
			RandomGenerator::GetRandomFloat(-cameraShakeSettings_.intensity.y,cameraShakeSettings_.intensity.y),
			RandomGenerator::GetRandomFloat(-cameraShakeSettings_.intensity.z,cameraShakeSettings_.intensity.z),
		};

		//カメラの座標に揺らし幅を加算する
		camera_.translation_ = cameraShakeSettings_.offset + randomValue;

		//シェイクタイマーが指定の時間を超えたらカメラシェイクを無効化する
		if (cameraShakeSettings_.timer > cameraShakeSettings_.duration)
		{
			cameraShakeSettings_.isShaking = false;
			cameraShakeSettings_.timer = 0.0f;
			camera_.translation_ = cameraShakeSettings_.offset;
		}
	}
}

Vector3 CameraController::Offset()
{
	//追従対象からのオフセット
	Vector3 offset = offset_;
	//回転行列の合成
	Matrix4x4 rotateXMatrix = Mathf::MakeRotateXMatrix(camera_.rotation_.x);
	Matrix4x4 rotateYMatrix = Mathf::MakeRotateYMatrix(camera_.rotation_.y);
	Matrix4x4 rotateZMatrix = Mathf::MakeRotateZMatrix(camera_.rotation_.z);
	Matrix4x4 rotateMatrix = rotateXMatrix * rotateYMatrix * rotateZMatrix;
	//オフセットをカメラの回転に合わせて回転させる
	offset = Mathf::TransformNormal(offset, rotateMatrix);
	return offset;
}

void CameraController::Reset()
{
	//追従対象がいれば
	if (target_) {
		//追従座標・角度の初期化
		interTarget_ = target_->translation_;
		camera_.rotation_.x = target_->rotation_.x;
		camera_.rotation_.y = target_->rotation_.y;
	}
	destinationAngleX_ = camera_.rotation_.x;
	destinationAngleY_ = camera_.rotation_.y;

	//追従対象からのオフセット
	Vector3 offset = Offset();
	camera_.translation_ = interTarget_ + offset;
}

void CameraController::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "CameraController";
	cameraShakeSettings_.duration = globalVariables->GetFloatValue(groupName, "CameraDuration");
	cameraShakeSettings_.intensity = globalVariables->GetVector3Value(groupName, "ShakeIntensity");
}

void CameraController::SetTarget(const WorldTransform* target)
{
	target_ = target;
	Reset();
}