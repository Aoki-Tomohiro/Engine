#include "CameraStateLockon.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Camera/States/CameraStateFollow.h"
#include "Application/Src/Object/Camera/States/CameraStateLaunchAttack.h"
#include "Application/Src/Object/Player/Player.h"

void CameraStateLockon::Initialize()
{
	//オフセット値の初期化
	cameraController_->SetDestinationOffset(cameraController_->GetLockonCameraParameters().offset);

    //カメラパスの初期化
    dashCameraPath_ = cameraController_->GetCameraPathManager()->GetPath("Dash");
}

void CameraStateLockon::Update()
{
    //カメラの回転を更新
    UpdateCameraRotation();

    //プレイヤーを取得
    const Player* player = GameObjectManager::GetInstance()->GetConstGameObject<Player>("");

    //プレイヤーがダッシュしている場合、ダッシュカメラアニメーションを適用
    HandleDashState(player);

    //カメラの状態遷移を管理
    ManageCameraStateTransition(player);
}

void CameraStateLockon::UpdateCameraRotation()
{
    //前のフレームの方向ベクトルを更新
    previousDirection_ = currentDirection_;

    //ロックオン対象の位置を取得
    Vector3 lockOnPosition = cameraController_->GetLockon()->GetTargetPosition();

    //現在のカメラ位置からロックオン対象までの方向ベクトルを計算
    currentDirection_ = Mathf::Normalize(lockOnPosition - cameraController_->GetPosition());
    
    //X軸の角度を計算
    float dotProduct = CalculateRotationAngle();

    //回転角度が設定された範囲内であればカメラの向きをロックオン対象に合わせる
    if (IsWithinRotationRange(dotProduct))
    {
        cameraController_->SetDestinationQuaternion(Mathf::LookAt(cameraController_->GetPosition(), lockOnPosition));
    }
    //回転角度が範囲外であり前の方向と現在の方向が異なる場合
    else if (previousDirection_ != currentDirection_)
    {
        //クォータニオンによる回転を計算
        Quaternion newQuaternion = CalculateNewRotation();
        //現在のクォータニオンに新しい回転を加算
        cameraController_->SetDestinationQuaternion(Mathf::Normalize(newQuaternion * cameraController_->GetDestinationQuaternion()));
    }
}

float CameraStateLockon::CalculateRotationAngle() const
{
    //X軸に対する回転角度を計算
    return std::acos(Mathf::Dot({ 0.0f, 1.0f, 0.0f }, { 0.0f, currentDirection_.y, 0.0f }));
}

bool CameraStateLockon::IsWithinRotationRange(const float angle) const
{
    //回転角度が設定された範囲内かどうかをチェック
    const auto& params = cameraController_->GetLockonCameraParameters();
    return angle > params.rotationRangeMin && angle < params.rotationRangeMax;
}

Quaternion CameraStateLockon::CalculateNewRotation() const
{
    //前の方向ベクトルと現在の方向ベクトルを正規化して水平面上の方向を取得
    Vector3 previous = Mathf::Normalize(Vector3{ previousDirection_.x, 0.0f, previousDirection_.z });
    Vector3 current = Mathf::Normalize(Vector3{ currentDirection_.x, 0.0f, currentDirection_.z });

    //正規化した前の方向ベクトルと現在の方向ベクトルのドット積を計算
    float dotProduct = Mathf::Dot(previous, current);

    //ドット積が-1から1の範囲に収まるように制限
    dotProduct = std::clamp(dotProduct, -1.0f, 1.0f);

    //ドット積から回転角度を計算
    float angle = std::acos(dotProduct);

    //前の方向ベクトルと現在の方向ベクトルのクロス積を計算
    Vector3 cross = Mathf::Cross(previous, current);

    //クロス積のY成分のドット積を計算して回転軸の方向を確認
    float direction = Mathf::Dot(cross, { 0.0f, 1.0f, 0.0f });

    //回転方向によってクォータニオンを生成
    return direction >= 0.0f
        ? Mathf::MakeRotateAxisAngleQuaternion({ 0.0f, 1.0f, 0.0f }, angle)
        : Mathf::MakeRotateAxisAngleQuaternion({ 0.0f, 1.0f, 0.0f }, -angle);
}

void CameraStateLockon::HandleDashState(const Player* player)
{
    //プレイヤーがダッシュ中であればダッシュカメラアニメーションを有効化
    if (player->GetActionFlag(Player::ActionFlag::kDashing))
    {
        //ダッシュカメラアニメーションを有効化
        if (!isDashCameraAnimationActive_)
        {
            isDashCameraAnimationActive_ = true;
        }
    }

    //ダッシュカメラアニメーションが有効であればアニメーションを適用
    if (isDashCameraAnimationActive_)
    {
        ApplyDashCameraAnimation();
    }
}

void CameraStateLockon::ApplyDashCameraAnimation()
{
    //アニメーション時間を更新
    UpdateAnimationTime();

    //現在のアニメーション時間に基づいてカメラのキーフレームを取得
    CameraKeyFrame cameraKeyFrame = dashCameraPath_.GetInterpolatedKeyFrame(dashAnimationTime_);

    //キーフレームに基づいてカメラの位置と回転を更新
    UpdateCameraTransformFromKeyFrame(cameraKeyFrame);

    //アニメーションが終了しているかどうかを確認し、必要に応じてカメラ状態を変更
    CheckAnimationCompletion();
}

void CameraStateLockon::UpdateAnimationTime()
{
    //アニメーション時間を進める
    dashAnimationTime_ += GameTimer::GetDeltaTime();

    //アニメーション時間をカメラパスの持続時間にクランプ
    dashAnimationTime_ = std::min<float>(dashAnimationTime_, dashCameraPath_.GetDuration());
}

void CameraStateLockon::UpdateCameraTransformFromKeyFrame(const CameraKeyFrame& keyFrame)
{
    //キーフレームの情報を基にカメラの位置オフセットを設定
    cameraController_->SetDestinationOffset(keyFrame.position);

    //キーフレームの回転情報を基にカメラの回転を設定
    cameraController_->SetDestinationQuaternion(keyFrame.rotation * cameraController_->GetDestinationQuaternion());

    //キーフレームの視野角情報を基にカメラのFOVを設定
    cameraController_->SetFov(keyFrame.fov * (std::numbers::pi_v<float> / 180.0f));
}

void CameraStateLockon::CheckAnimationCompletion()
{
    //アニメーションが終了している場合
    if (dashAnimationTime_ >= dashCameraPath_.GetDuration())
    {
        isDashCameraAnimationActive_ = false;
        dashAnimationTime_ = 0.0f;
    }
}

void CameraStateLockon::ManageCameraStateTransition(const Player* player)
{
    //ロックオン対象がいない場合、追従カメラに切り替え
    if (!cameraController_->GetLockon()->ExistTarget())
    {
        cameraController_->ChangeState(new CameraStateFollow());
    }
    //プレイヤーが打ち上げ攻撃中であれば、打ち上げ攻撃カメラに切り替え
    else if (player->GetActionFlag(Player::ActionFlag::kLaunchAttack))
    {
        cameraController_->ChangeState(new CameraStateLaunchAttack());
    }
}