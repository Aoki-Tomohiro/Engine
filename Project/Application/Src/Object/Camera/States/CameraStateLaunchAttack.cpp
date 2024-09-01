#include "CameraStateLaunchAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Camera/States/CameraStateFollow.h"
#include "Application/Src/Object/Camera/States/CameraStateLockOn.h"
#include "Application/Src/Object/Player/Player.h"

void CameraStateLaunchAttack::Initialize()
{
	//カメラパスの初期化
	cameraPath_ = cameraController_->GetCameraPathManager()->GetPath("LaunchAttack");
}

void CameraStateLaunchAttack::Update()
{
	//プレイヤーを取得
	Player* player = GameObjectManager::GetInstance()->GetMutableGameObject<Player>("");

	//アニメーション時間を更新
	UpdateAnimationTime();

	//現在のアニメーション時間に基づいてカメラのキーフレームを取得
	CameraKeyFrame cameraKeyFrame = cameraPath_.GetInterpolatedKeyFrame(animationTime);

	//キーフレームに基づいてカメラの位置と回転を更新
	UpdateCameraTransform(cameraKeyFrame, player);

	//アニメーションが終了しているかどうかを確認し、必要に応じてカメラ状態を変更
	CheckAnimationCompletion();
}

void CameraStateLaunchAttack::UpdateAnimationTime()
{
	//アニメーション時間を進める
	animationTime += GameTimer::GetDeltaTime();

	//アニメーション時間をカメラパスの持続時間にクランプ
	animationTime = std::min<float>(animationTime, cameraPath_.GetDuration());
}

void CameraStateLaunchAttack::UpdateCameraTransform(const CameraKeyFrame& keyFrame, Player* player)
{
	//キーフレームの情報を基にカメラの位置オフセットを設定
	cameraController_->SetDestinationOffset(keyFrame.position);

	//キーフレームの回転情報を基にカメラの回転を設定
	cameraController_->SetDestinationQuaternion(keyFrame.rotation * player->GetDestinationQuaternion());

	//キーフレームの視野角情報を基にカメラのFOVを設定
	cameraController_->SetFov(keyFrame.fov * (std::numbers::pi_v<float> / 180.0f));
}

void CameraStateLaunchAttack::CheckAnimationCompletion()
{
	//アニメーションが終了している場合
	if (animationTime >= cameraPath_.GetDuration())
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