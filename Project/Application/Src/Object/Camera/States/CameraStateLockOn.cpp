#include "CameraStateLockon.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Camera/States/CameraStateFollow.h"

void CameraStateLockon::Initialize()
{
	//オフセット値の初期化
	cameraController_->SetDestinationOffset(cameraController_->GetLockonCameraParameters().offset);
}

void CameraStateLockon::Update()
{
    //カメラの回転を更新
    UpdateCameraRotation();

    //追従カメラに遷移
    if (!cameraController_->GetLockon()->ExistTarget())
    {
        cameraController_->ChangeState(new CameraStateFollow());
    }
}

void CameraStateLockon::UpdateCameraRotation()
{
    //ロックオン対象の座標を取得
    Vector3 lockOnPosition = cameraController_->GetLockon()->GetTargetPosition();

    //現在のカメラ位置からロックオン対象までの差分ベクトルを計算
    Vector3 direction = Mathf::Normalize(lockOnPosition - cameraController_->GetPosition());

    //X軸の角度を計算
    float dot = std::acos(Mathf::Dot({ 0.0f, 1.0f, 0.0f }, { 0.0f, direction.y, 0.0f }));

    //X軸の角度が一定範囲内の場合はカメラの向きを更新
    if (dot > cameraController_->GetLockonCameraParameters().rotationRangeMin && dot < cameraController_->GetLockonCameraParameters().rotationRangeMax)
    {
        cameraController_->SetDestinationQuaternion(Mathf::LookAt(cameraController_->GetPosition(), lockOnPosition));
    }
}