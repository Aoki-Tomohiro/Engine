#include "CameraStateFollow.h"
#include "Application/Src/Object/CameraController/CameraController.h"
#include "Application/Src/Object/CameraController/States/CameraStateLockOn.h"
#include <numbers>

void CameraStateFollow::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();
}

void CameraStateFollow::Update()
{
	//追従対象からのオフセットを取得してカメラ座標を計算する
	Vector3 offset = Offset();
	cameraController_->camera_.translation_ = cameraController_->interTarget_ + offset;

	//スティックの入力のしきい値
	const float threshold = 0.7f;

	//右スティックの入力を取得
	Vector3 inputValue = {
		input_->GetRightStickY(),
		input_->GetRightStickX(),
		0.0f
	};

	//スティックの入力が遊び範囲を超えていた場合
	if (Mathf::Length(inputValue) > threshold)
	{
		//入力値に速度を掛ける
		cameraController_->camera_.rotation_.x += inputValue.x * 0.02f;
		cameraController_->camera_.rotation_.y += inputValue.y * 0.04f;

		//回転を2πの範囲内に収める
		NormalizeRotation(cameraController_->camera_.rotation_.x);
		NormalizeRotation(cameraController_->camera_.rotation_.y);

		//Quaternionの生成
		Quaternion rotationQuaternion = Mathf::MakeRotateAxisAngleQuaternion({ 1.0f,0.0f,0.0f }, -cameraController_->camera_.rotation_.x);
		rotationQuaternion = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, cameraController_->camera_.rotation_.y) * rotationQuaternion;

		//回転処理
		cameraController_->destinationQuaternion_ = Mathf::Normalize(rotationQuaternion);
	}

	//カメラの状態を遷移
	if (cameraController_->lockOn_->ExistTarget())
	{
		cameraController_->ChangeState(new CameraStateLockOn());
	}
}

Vector3 CameraStateFollow::Offset()
{
	//追従対象からのオフセット
	Vector3 offset = offset_;
	//回転行列の合成
	Matrix4x4 rotateMatrix = Mathf::MakeRotateMatrix(cameraController_->camera_.quaternion_);
	//オフセットをカメラの回転に合わせて回転させる
	offset = Mathf::TransformNormal(offset, rotateMatrix);
	//オフセット値を返す
	return offset;
}

void CameraStateFollow::Reset()
{
	//追従対象がいれば
	if (cameraController_->target_) {
		//追従座標・角度の初期化
		cameraController_->interTarget_ = cameraController_->target_->translation_;
		cameraController_->camera_.quaternion_ = cameraController_->target_->quaternion_;
	}
	cameraController_->destinationQuaternion_ = cameraController_->camera_.quaternion_;

	//追従対象からのオフセット
	Vector3 offset = Offset();
	cameraController_->camera_.translation_ = cameraController_->interTarget_ + offset;
}

void CameraStateFollow::NormalizeRotation(float& rotation)
{
	rotation = std::fmod(rotation, std::numbers::pi_v<float> * 2.0f);
	if (rotation < 0) 
	{
		rotation += std::numbers::pi_v<float> *2.0f;
	}
}