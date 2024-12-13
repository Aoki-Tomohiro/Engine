/**
 * @file CameraStateDebug.cpp
 * @brief カメラのデバッグ状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#include "CameraStateDebug.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Camera/States/CameraStateFollow.h"
#include "Application/Src/Object/Camera/States/CameraStateLockon.h"
#include "Application/Src/Object/Camera/States/CameraStateAnimation.h"
#include <numbers>

void CameraStateDebug::Update()
{
	//カメラアニメーションエディタの取得
	CameraAnimationEditor* cameraAnimationEditor = cameraController_->GetCameraAnimationEditor();

	//現在のキーフレームを取得
	const CameraPath::CameraKeyFrame& keyFrame = cameraAnimationEditor->GetNewKeyFrame();

	//キーフレームの情報を基にカメラの位置オフセットを設定
	cameraController_->SetOffset(keyFrame.position);

	//キーフレームの回転情報を基にカメラの回転を設定
	cameraController_->SetDestinationQuaternion(keyFrame.rotation);

	//キーフレームの視野角情報を基にカメラのFOVを設定
	cameraController_->SetFov(keyFrame.fov * (std::numbers::pi_v<float> / 180.0f));

	//デバッグのフラグが解除されたら通常状態に戻す
	if (!cameraAnimationEditor->GetIsDebug())
	{
		//ロックオンに応じて遷移する状態を変える
		cameraController_->GetLockon()->ExistTarget() ? cameraController_->ChangeState(new CameraStateLockon()) : cameraController_->ChangeState(new CameraStateFollow());
	}
	//現在編集中のカメラアニメーションが再生されたらアニメーション状態に遷移
	else if (cameraAnimationEditor->GetIsPlayingCurrentCameraAnimation())
	{
		cameraController_->ChangeState(new CameraStateAnimation(cameraAnimationEditor->GetCurrentEditPathName(), 1.0f, false));
	}
}