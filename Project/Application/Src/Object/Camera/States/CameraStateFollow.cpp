#include "CameraStateFollow.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Camera/States/CameraStateLockon.h"
#include "Application/Src/Object/Player/Player.h"

void CameraStateFollow::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//オフセット値の初期化
	cameraController_->SetDestinationOffset(cameraController_->GetFollowCameraParameters().offset);

	//Quaternionの初期化
	if (const Player* player = GameObjectManager::GetInstance()->GetConstGameObject<Player>("Player"))
	{
		quaternionY = player->GetDestinationQuaternion();
		cameraController_->SetDestinationQuaternion(quaternionY);
	}
}

void CameraStateFollow::Update()
{
	//カメラの回転を更新
	UpdateCameraRotation();

	//ロックオンカメラに遷移
	if (cameraController_->GetLockon()->ExistTarget() && !IsPlayerPerformingAction())
	{
		cameraController_->ChangeState(new CameraStateLockon());
	}
}

const bool CameraStateFollow::IsPlayerPerformingAction() const
{
	//プレイヤーを取得
	const Player* player = GameObjectManager::GetInstance()->GetConstGameObject<Player>("Player");

	//いずれかのフラグが立っていたらtrueを返す
	return player->GetActionFlag(Player::ActionFlag::kDashing) ||
		player->GetActionFlag(Player::ActionFlag::kLaunchAttack) ||
		player->GetActionFlag(Player::ActionFlag::kFallingAttack);
}

void CameraStateFollow::UpdateCameraRotation()
{
	//右スティックの入力値を取得
	Vector3 input = GetRightStickInput();

	//追従対象の位置を取得
	Vector3 targetPosition = cameraController_->GetInterTarget();

	//現在のカメラ位置から追従対象までの方向ベクトルを計算
	Vector3 cameraToTargetDir = Mathf::Normalize(targetPosition - cameraController_->GetPosition());

	//X軸の回転を制限
	ClampVerticalRotation(input, cameraToTargetDir);

	//スティック入力によるカメラ回転処理のしきい値
	const float threshold = 0.7f;

	//スティック入力の大きさがしきい値を超えている場合のみカメラ回転を適用
	if (Mathf::Length(input) > threshold)
	{
		ApplyCameraRotation(input);
	}
}

void CameraStateFollow::ClampVerticalRotation(Vector3& input, const Vector3& direction)
{
	//カメラの垂直角度を計算
	float verticalAngle = std::acos(Mathf::Dot({ 0.0f, 1.0f, 0.0f }, { 0.0f, direction.y, 0.0f }));

	//垂直角度に応じて入力値を制限（カメラが特定の角度を超えないようにする）
	if (verticalAngle < cameraController_->GetFollowCameraParameters().rotationRangeMin && input.x >= 0.0f)
	{
		input.x = 0.0f;
	}
	else if (verticalAngle > cameraController_->GetFollowCameraParameters().rotationRangeMax && input.x <= 0.0f)
	{
		input.x = 0.0f;
	}
}

void CameraStateFollow::ApplyCameraRotation(const Vector3& input)
{
	//スティック入力に基づいてカメラの回転量を計算
	Vector3 rotation = {
		input.x * cameraController_->GetFollowCameraParameters().rotationSpeedX,
		input.y * cameraController_->GetFollowCameraParameters().rotationSpeedY,
		0.0f,
	};

	//X軸回転のクォータニオンを作成
	Quaternion newQuaternionX = Mathf::MakeRotateAxisAngleQuaternion({ 1.0f, 0.0f, 0.0f }, -rotation.x);

	//Y軸回転のクォータニオンを作成
	Quaternion newQuaternionY = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f, 1.0f, 0.0f }, rotation.y);

	//新しい回転を現在のクォータニオンに適用
	quaternionX = newQuaternionX * quaternionX;
	quaternionY = newQuaternionY * quaternionY;

	//X軸とY軸の回転クォータニオンを合成
	Quaternion combinedRotation = quaternionY * quaternionX;

	//合成された回転クォータニオンを正規化してカメラの目的クォータニオンに設定
	cameraController_->SetDestinationQuaternion(Mathf::Normalize(combinedRotation));
}

Vector3 CameraStateFollow::GetRightStickInput() const
{
	//右スティックの入力値を取得
	return {
		input_->GetRightStickY(),
		input_->GetRightStickX(),
		0.0f
	};
}