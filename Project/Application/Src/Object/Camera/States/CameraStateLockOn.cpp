#include "CameraStateLockon.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Camera/States/CameraStateFollow.h"
#include "Application/Src/Object/Camera/States/CameraStateDash.h"
#include "Application/Src/Object/Camera/States/CameraStateLaunchAttack.h"
#include "Application/Src/Object/Camera/States/CameraStateFallingAttack.h"
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

    //カメラの状態遷移を管理
    ManageCameraStateTransition();
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

void CameraStateLockon::ManageCameraStateTransition()
{
    //プレイヤーを取得
    const Player* player = GameObjectManager::GetInstance()->GetConstGameObject<Player>("Player");

    //ロックオン対象がいない場合、追従カメラに切り替え
    if (!cameraController_->GetLockon()->ExistTarget())
    {
        cameraController_->ChangeState(new CameraStateFollow());
    }
    //プレイヤーがダッシュ状態であれば、ダッシュカメラに切り替え
    else if (player->GetActionFlag(Player::ActionFlag::kDashing))
    {
        cameraController_->ChangeState(new CameraStateDash());
    }
    //プレイヤーが打ち上げ攻撃中であれば、打ち上げ攻撃カメラに切り替え
    else if (player->GetActionFlag(Player::ActionFlag::kLaunchAttack))
    {
        cameraController_->ChangeState(new CameraStateLaunchAttack());
    }
    //プレイヤーが落下攻撃中であれば、落下攻撃カメラに切り替え
    else if (player->GetActionFlag(Player::ActionFlag::kFallingAttack))
    {
        cameraController_->ChangeState(new CameraStateFallingAttack());
    }
}