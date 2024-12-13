/**
 * @file CameraStateFollow.cpp
 * @brief 追従カメラの状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#include "CameraStateFollow.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Camera/States/CameraStateLockon.h"
#include "Application/Src/Object/Camera/States/CameraStateDebug.h"

void CameraStateFollow::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//オフセット値の初期化
	cameraController_->SetOffset(cameraController_->GetFollowCameraParameters().offset);

	//FOVの初期化
	cameraController_->SetFov(45.0f * (std::numbers::pi_v<float> / 180.0f));
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

	//追従対象の位置を取得
	Vector3 targetPosition = cameraController_->GetInterTarget();

	//現在のカメラ位置から追従対象までの方向ベクトルを計算
	Vector3 cameraToTargetDir = Mathf::Normalize(targetPosition - cameraController_->GetCameraPosition());

	//X軸の回転を制限
	ClampVerticalRotation(inputValue, cameraToTargetDir);

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
	//デバッグのフラグが立っていた場合はデバッグ状態に遷移
	else if (cameraController_->GetCameraAnimationEditor()->GetIsDebug())
	{
		cameraController_->ChangeState(new CameraStateDebug());
	}
}

void CameraStateFollow::ClampVerticalRotation(Vector3& inputValue, const Vector3& direction)
{
	//カメラの垂直角度を計算
	float verticalAngle = std::acos(Mathf::Dot({ 0.0f, 1.0f, 0.0f }, { 0.0f, direction.y, 0.0f }));

	//垂直角度に応じて入力値を制限（カメラが特定の角度を超えないようにする）
	if (verticalAngle < cameraController_->GetFollowCameraParameters().rotationRangeMin && inputValue.x >= 0.0f)
	{
		inputValue.x = 0.0f;
	}
	else if (verticalAngle > cameraController_->GetFollowCameraParameters().rotationRangeMax && inputValue.x <= 0.0f)
	{
		inputValue.x = 0.0f;
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

	//Y軸のクォータニオンをワールド空間で適用
	Quaternion newQuaternionY = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f, 1.0f, 0.0f }, rotation.y) * currentQuaternion;

	//X軸のクォータニオンをローカル空間で適用
	Quaternion newQuaternionX = newQuaternionY * Mathf::MakeRotateAxisAngleQuaternion({ 1.0f, 0.0f, 0.0f }, -rotation.x);

	//合成された回転クォータニオンをカメラに設定
	cameraController_->SetDestinationQuaternion(newQuaternionX);
}