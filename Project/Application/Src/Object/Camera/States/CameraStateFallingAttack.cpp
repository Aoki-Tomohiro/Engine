#include "CameraStateFallingAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Camera/States/CameraStateFollow.h"
#include "Application/Src/Object/Camera/States/CameraStateLockOn.h"
#include "Application/Src/Object/Character/Player/Player.h"

void CameraStateFallingAttack::Initialize()
{
	//カメラパスの初期化
	cameraPath_ = cameraController_->GetCameraAnimationEditor()->GetCameraPath("FallingAttack");
}

void CameraStateFallingAttack::Update()
{
	//プレイヤーを取得
	Player* player = GameObjectManager::GetInstance()->GetGameObject<Player>("Player");

	//アニメーション時間を更新
	UpdateAnimationTime(player);

	//現在のアニメーション時間に基づいてカメラのキーフレームを取得
	CameraKeyFrame cameraKeyFrame = cameraPath_.GetInterpolatedKeyFrame(animationTime_);

	//キーフレームに基づいてカメラの位置と回転を更新
	UpdateCameraTransform(cameraKeyFrame, player);

	//アニメーションが終了しているかどうかを確認し、必要に応じてカメラ状態を変更
	CheckAnimationCompletion(player);
}

void CameraStateFallingAttack::UpdateAnimationTime(Player* player)
{
	//プレイヤーのアニメーターを取得
	AnimatorComponent* animator = player->GetAnimator();

	//プレイヤーのアニメーション時間を設定
	animationTime_ = animator->GetIsBlendingCompleted() ? animator->GetCurrentAnimationTime() : animator->GetNextAnimationTime();

	//アニメーション時間をカメラパスの持続時間にクランプ
	animationTime_ = std::min<float>(animationTime_, cameraPath_.GetDuration());
}

void CameraStateFallingAttack::UpdateCameraTransform(const CameraKeyFrame& keyFrame, Player* player)
{
	//キーフレームの情報を基にカメラの位置オフセットを設定
	cameraController_->SetDestinationOffset(keyFrame.position);

	//キーフレームの回転情報を基にカメラの回転を設定
	cameraController_->SetDestinationQuaternion(player->GetQuaternion() * keyFrame.rotation);

	//キーフレームの視野角情報を基にカメラのFOVを設定
	cameraController_->SetFov(keyFrame.fov * (std::numbers::pi_v<float> / 180.0f));
}

void CameraStateFallingAttack::CheckAnimationCompletion(Player* player)
{
	//落下攻撃が終了している場合
	if (!player->GetActionFlag(Player::ActionFlag::kFallingAttack))
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