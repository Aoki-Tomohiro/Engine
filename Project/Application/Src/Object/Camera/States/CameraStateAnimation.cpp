#include "CameraStateAnimation.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Camera/States/CameraStateFollow.h"
#include "Application/Src/Object/Camera/States/CameraStateLockon.h"
#include "Application/Src/Object/Camera/States/CameraStateDebug.h"
#include <numbers>

void CameraStateAnimation::Initialize()
{
	//カメラパスを取得
	cameraPath_ = cameraController_->GetCameraAnimationEditor()->GetCameraPath(animationName_);

	//追従対象のクォータニオンを取得
	followTargetQuaternion_ = cameraController_->GetTarget()->worldTransform_.quaternion_;
}

void CameraStateAnimation::Update()
{
	//アニメーションの時間を更新
	UpdateAnimationTime();

	//現在のアニメーション時間に基づいてカメラのキーフレームを取得
	CameraPath::CameraKeyFrame cameraKeyFrame = cameraPath_.GetInterpolatedKeyFrame(animationTime_);

	//キーフレームに基づいてカメラの位置と回転を更新
	UpdateCameraTransform(cameraKeyFrame);

	//アニメーションが終了している場合、次の状態へ遷移
	if (animationTime_ >= cameraPath_.GetDuration())
	{
		//カメラを通常状態に戻す
		cameraController_->StartInterpolationReset(cameraPath_.GetResetEasingType(), cameraPath_.GetResetDuration());

		//デバッグのフラグが立っていた場合はデバッグ状態に遷移
		if (cameraController_->GetCameraAnimationEditor()->GetIsDebug())
		{
			cameraController_->ChangeState(new CameraStateDebug());
		}
		//デバッグのフラグが立っていなかった場合はロックオンに応じて遷移する状態を変える
		else
		{
			cameraController_->GetLockon()->ExistTarget() ? cameraController_->ChangeState(new CameraStateLockon()) : cameraController_->ChangeState(new CameraStateFollow());
		}
	}
}

void CameraStateAnimation::UpdateAnimationTime()
{
	//プレイヤーのアニメーションと同期して進行時間を取得
	if (syncWithCharacterAnimation_)
	{
		//プレイヤーのアニメーションコンポーネントを取得
		AnimatorComponent* animator = GameObjectManager::GetInstance()->GetGameObject<Player>("Player")->GetAnimator();

		//アニメーションのブレンド状態に基づき、現在または次のアニメーション時間を使用
		animationTime_ = animator->GetIsBlendingCompleted() ? animator->GetCurrentAnimationTime() : animator->GetNextAnimationTime();
	}
	//独立したアニメーション時間を進行
	else
	{
		//アニメーション速度に応じて独立したアニメーション時間を進行
		animationTime_ += animationSpeed_ * GameTimer::GetDeltaTime();
	}

	//カメラパスの持続時間内にアニメーション時間をクランプ
	animationTime_ = std::min<float>(animationTime_, cameraPath_.GetDuration());
}

void CameraStateAnimation::UpdateCameraTransform(const CameraPath::CameraKeyFrame& keyFrame)
{
	//キーフレームの情報を基にカメラの位置オフセットを設定
	cameraController_->SetOffset(keyFrame.position);

	//キーフレームの回転情報を基にカメラの回転を設定
	cameraController_->SetDestinationQuaternion(followTargetQuaternion_ * keyFrame.rotation);

	//キーフレームの視野角情報を基にカメラのFOVを設定
	cameraController_->SetFov(keyFrame.fov * (std::numbers::pi_v<float> / 180.0f));
}