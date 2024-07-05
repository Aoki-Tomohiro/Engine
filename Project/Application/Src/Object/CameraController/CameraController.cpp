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
	//カメラの初期化
	camera_.Initialize();

	//環境変数の設定
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "CameraController";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "ShakeDuration", shakeDuration_);
	globalVariables->AddItem(groupName, "ShakeIntensity", shakeIntensity_);
};

void CameraController::Update()
{
	//FollowCameraの更新
	UpdateFollowCamera();

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

	//カメラの更新
	camera_.UpdateMatrix();
};

void CameraController::UpdateFollowCamera()
{
	//追従対象がいれば
	if (target_)
	{
		//追従座標の補間
		interTarget_ = Mathf::Lerp(interTarget_, target_->translation_, 0.2f);
	}

	//追従対象からのオフセット
	Vector3 offset = Offset();
	//カメラ座標
	camera_.translation_ = interTarget_ + offset;

	//ロックオン中なら
	if (lockOn_ && lockOn_->ExistTarget())
	{
		//ロックオン座標
		Vector3 lockOnPosition = lockOn_->GetTargetPosition();
		//追従対象からロックオン座標へのベクトル
		Vector3 sub = lockOnPosition - target_->translation_;
		sub.y = 0.0f;

		//Y軸周り角度
		if (sub.z != 0.0) {
			destinationAngleY_ = std::asin(sub.x / std::sqrt(sub.x * sub.x + sub.z * sub.z));

			if (sub.z < 0.0) {
				destinationAngleY_ = (sub.x >= 0.0) ? std::numbers::pi_v<float> -destinationAngleY_ : -std::numbers::pi_v<float> -destinationAngleY_;
			}
		}
		else {
			destinationAngleY_ = (sub.x >= 0.0) ? std::numbers::pi_v<float> / 2.0f : -std::numbers::pi_v<float> / 2.0f;
		}

		destinationAngleX_ = 0.2f;
	}
	else
	{
		//しきい値
		const float threshold = 0.7f;

		//回転フラグ
		bool isRotation = false;

		//回転量
		Vector3 rotation = {
			Input::GetInstance()->GetRightStickY(),
			Input::GetInstance()->GetRightStickX(),
			0.0f
		};

		//スティックの押し込みが遊び範囲を超えていたら回転フラグをtureにする
		if (Mathf::Length(rotation) > threshold)
		{
			isRotation = true;
		}

		if (isRotation)
		{
			//回転速度
			const float kRotSpeedX = 0.02f;
			const float kRotSpeedY = 0.04f;

			//回転
			destinationAngleX_ -= rotation.x * kRotSpeedX;
			destinationAngleY_ += rotation.y * kRotSpeedY;
		}
	}
}

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
			isShaking_ = true;
			shakeoffset_ = camera_.translation_;
		}
	}

	//カメラシェイクが有効な場合
	if (isShaking_)
	{
		//シェイクタイマーを進める
		shakeTimer_ += GameTimer::GetDeltaTime();

		//揺らし幅をランダムで決める
		Vector3 randomValue = {
			RandomGenerator::GetRandomFloat(-shakeIntensity_.x,shakeIntensity_.x),
			RandomGenerator::GetRandomFloat(-shakeIntensity_.y,shakeIntensity_.y),
			RandomGenerator::GetRandomFloat(-shakeIntensity_.z,shakeIntensity_.z),
		};

		//カメラの座標に揺らし幅を加算する
		camera_.translation_ = shakeoffset_ + randomValue;

		//シェイクタイマーが指定の時間を超えたらカメラシェイクを無効化する
		if (shakeTimer_ > shakeDuration_)
		{
			isShaking_ = false;
			shakeTimer_ = 0.0f;
			camera_.translation_ = shakeoffset_;
		}
	}
}

Vector3 CameraController::Offset()
{
	//追従対象からのオフセット
	Vector3 offset = { 0.0f, 2.0f, -20.0f };
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
	shakeDuration_ = globalVariables->GetFloatValue(groupName, "CameraDuration");
	shakeIntensity_ = globalVariables->GetVector3Value(groupName, "ShakeIntensity");
}

void CameraController::SetTarget(const WorldTransform* target)
{
	target_ = target;
	Reset();
}