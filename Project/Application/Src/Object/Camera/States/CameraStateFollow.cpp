#include "CameraStateFollow.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Camera/States/CameraStateLockon.h"

void CameraStateFollow::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//オフセット値の初期化
	cameraController_->SetDestinationOffset(cameraController_->GetFollowCameraParameters().offset);
}

void CameraStateFollow::Update()
{
	//スティック入力によるカメラ回転処理の閾値
	const float threshold = 0.7f;

	//スティックの入力値を取得
	Vector3 inputValue = {
		input_->GetRightStickY(),
		input_->GetRightStickX(),
		0.0f
	};

	//スティック入力の大きさがしきい値を超えている場合のみカメラ回転を適用
	if (Mathf::Length(inputValue) > threshold)
	{
		ApplyCameraRotation(inputValue);
	}

	//ロックオンカメラに遷移
	if (cameraController_->GetLockon()->ExistTarget())
	{
		cameraController_->ChangeState(new CameraStateLockon());
	}
}

void CameraStateFollow::ApplyCameraRotation(const Vector3& inputValue)
{
	//スティック入力に基づいてカメラの回転量を計算
	Vector3 rotation = {
		inputValue.x * cameraController_->GetFollowCameraParameters().rotationSpeedX,
		inputValue.y * cameraController_->GetFollowCameraParameters().rotationSpeedY,
		0.0f,
	};

	//現在のクォータニオンを取得
	const Quaternion& currentQuaternion = cameraController_->GetDestinationQuaternion();

	//Y軸（水平回転）のクォータニオンをワールド空間で適用
	Quaternion newQuaternionY = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f, 1.0f, 0.0f }, rotation.y) * currentQuaternion;

	//X軸（垂直回転）のクォータニオンをローカル空間で適用
	Quaternion newQuaternionX = newQuaternionY * Mathf::MakeRotateAxisAngleQuaternion({ 1.0f, 0.0f, 0.0f }, -rotation.x);

	//合成された回転クォータニオンをカメラに設定
	cameraController_->SetDestinationQuaternion(newQuaternionX);
}