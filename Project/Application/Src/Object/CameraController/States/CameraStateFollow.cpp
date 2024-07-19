#include "CameraStateFollow.h"
#include "Engine/Base/ImGuiManager.h"
#include "Application/Src/Object/CameraController/CameraController.h"

void CameraStateFollow::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();
}

void CameraStateFollow::Update()
{
	//追従対象が存在する場合、追従座標を補間する
	if (cameraController_->target_)
	{
		interTarget_ = Mathf::Lerp(interTarget_, cameraController_->target_->translation_, 0.2f);
	}

	//追従対象からのオフセットを取得してカメラ座標を計算する
	Vector3 offset = Offset();
	cameraController_->camera_.translation_ = interTarget_ + offset;

	//スティックの入力のしきい値
	const float threshold = 0.7f;

	//右スティックの入力を取得
	Vector3 rotation = {
		input_->GetRightStickY(),
		input_->GetRightStickX(),
		0.0f
	};

	//スティックの入力が遊び範囲を超えていた場合
	if (Mathf::Length(rotation) > threshold)
	{
		//入力値に速度を掛ける
		yaw_ += rotation.y * 0.04f;
		pitch_ += rotation.x * 0.02f;

		//Quaternionの生成
		Quaternion yawQuaternion = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, yaw_);
		Quaternion pitchQuaternion = Mathf::MakeRotateAxisAngleQuaternion({ 1.0f,0.0f,0.0f }, pitch_);
		Quaternion rotationQuaternion = yawQuaternion * pitchQuaternion;

		//回転処理
		cameraController_->destinationQuaternion_ = Mathf::Normalize(rotationQuaternion);
	}

	ImGui::Begin("CameraController");
	ImGui::DragFloat("Yaw", &yaw_, 0.01f);
	ImGui::DragFloat("Pitch", &pitch_, 0.01f);
	ImGui::End();
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
		interTarget_ = cameraController_->target_->translation_;
		cameraController_->camera_.quaternion_ = cameraController_->target_->quaternion_;
	}
	cameraController_->destinationQuaternion_ = cameraController_->camera_.quaternion_;

	//追従対象からのオフセット
	Vector3 offset = Offset();
	cameraController_->camera_.translation_ = interTarget_ + offset;
}