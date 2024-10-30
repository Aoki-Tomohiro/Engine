#include "CameraStateDash.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Camera/States/CameraStateFollow.h"
#include "Application/Src/Object/Camera/States/CameraStateLockOn.h"

void CameraStateDash::Initialize()
{
	//カメラパスの初期化
	cameraPath_ = cameraController_->GetCameraAnimationEditor()->GetCameraPath("Dash");
}

void CameraStateDash::Update()
{
    //アニメーション時間を更新
    UpdateAnimationTime();

    //現在のアニメーション時間に基づいてカメラのキーフレームを取得
    CameraKeyFrame cameraKeyFrame = cameraPath_.GetInterpolatedKeyFrame(animationTime_);

    //キーフレームに基づいてカメラの位置と回転を更新
    UpdateCameraTransformFromKeyFrame(cameraKeyFrame);

    //アニメーションが終了しているかどうかを確認し、必要に応じてカメラ状態を変更
    CheckAnimationCompletion();
}

void CameraStateDash::UpdateAnimationTime()
{
    //アニメーション時間を進める
    animationTime_ += GameTimer::GetDeltaTime();

    //アニメーション時間をカメラパスの持続時間にクランプ
    animationTime_ = std::min<float>(animationTime_, cameraPath_.GetDuration());
}

void CameraStateDash::UpdateCameraTransformFromKeyFrame(const CameraKeyFrame& keyFrame)
{
    //キーフレームの情報を基にカメラの位置オフセットを設定
    cameraController_->SetDestinationOffset(keyFrame.position);

    //キーフレームの回転情報を基にカメラの回転を設定
    cameraController_->SetDestinationQuaternion(keyFrame.rotation * cameraController_->GetDestinationQuaternion());

    //キーフレームの視野角情報を基にカメラのFOVを設定
    cameraController_->SetFov(keyFrame.fov * (std::numbers::pi_v<float> / 180.0f));
}

void CameraStateDash::CheckAnimationCompletion()
{
    //アニメーションが終了している場合
    if (animationTime_ >= cameraPath_.GetDuration())
    {
        //ターゲットが存在しない場合は追従カメラに遷移
        if (!cameraController_->GetLockon()->ExistTarget())
        {
            cameraController_->ChangeState(new CameraStateFollow());
        }
        //ターゲットが存在する場合はロックオンカメラに遷移
        else
        {
            cameraController_->ChangeState(new CameraStateLockon());
        }
    }
}