#include "CameraStateClear.h"
#include "Application/Src/Object/Camera/CameraController.h"

void CameraStateClear::Initialize()
{
	//カメラパスの初期化
	cameraPath_ = cameraController_->GetCameraAnimationEditor()->GetCameraPath("Clear");
}

void CameraStateClear::Update()
{
	//アニメーション時間を更新
	UpdateAnimationTime();

	//現在のアニメーション時間に基づいてカメラのキーフレームを取得
	CameraKeyFrame cameraKeyFrame = cameraPath_.GetInterpolatedKeyFrame(animationTime);

	//キーフレームに基づいてカメラの位置と回転を更新
	UpdateCameraTransform(cameraKeyFrame);

	//アニメーションが終了しているかどうかを確認し、必要に応じてカメラ状態を変更
	CheckAnimationCompletion();
}

void CameraStateClear::UpdateAnimationTime()
{
	//アニメーション時間を進める
	const float kDeltaTime = 1.0f / 60.0f;
	animationTime += kDeltaTime;

	//アニメーション時間をカメラパスの持続時間にクランプ
	animationTime = std::min<float>(animationTime, cameraPath_.GetDuration());
}

void CameraStateClear::UpdateCameraTransform(const CameraKeyFrame& keyFrame)
{
	//プレイヤーを取得
	const Player* player = cameraController_->GetTarget();

	//キーフレームの情報を基にカメラの位置オフセットを設定
	cameraController_->SetDestinationOffset(keyFrame.position);

	//キーフレームの回転情報を基にカメラの回転を設定
	cameraController_->SetDestinationQuaternion(keyFrame.rotation * player->GetQuaternion());

	//キーフレームの視野角情報を基にカメラのFOVを設定
	cameraController_->SetFov(keyFrame.fov * (std::numbers::pi_v<float> / 180.0f));
}

void CameraStateClear::CheckAnimationCompletion()
{
	//アニメーションが終了している場合
	if (animationTime >= cameraPath_.GetDuration())
	{
		//クリアアニメーションの終了フラグを立てる
		cameraController_->SetIsClearAnimationFinished(true);

		//ゲームの進行を元に戻す
		GameTimer::SetTimeScale(1.0f);
	}
	else
	{
		//ゲームの進行を遅くする
		GameTimer::SetTimeScale(0.1f);
	}
}