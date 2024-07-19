#include "CameraStateLockOn.h"
#include "Application/Src/Object/CameraController/CameraController.h"
#include "Application/Src/Object/CameraController/States/CameraStateFollow.h"

void CameraStateLockOn::Initialize()
{
}

void CameraStateLockOn::Update()
{
	//追従対象からのオフセットを取得してカメラ座標を計算する
	Vector3 offset = Offset();
	cameraController_->camera_.translation_ = cameraController_->interTarget_ + offset;

	//ロックオン対象の座標を取得
	Vector3 lockOnPosition = cameraController_->lockOn_->GetTargetPosition();

    //差分ベクトルを計算
	Vector3 sub = Mathf::Normalize(lockOnPosition - cameraController_->target_->translation_);

    //Quaternionの作成
	cameraController_->destinationQuaternion_ = Mathf::LookRotation(sub, { 0.0f,1.0f,0.0f });

	//カメラの状態を遷移
	if (!cameraController_->lockOn_->ExistTarget())
	{
		cameraController_->ChangeState(new CameraStateFollow());
	}
}

Vector3 CameraStateLockOn::Offset()
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