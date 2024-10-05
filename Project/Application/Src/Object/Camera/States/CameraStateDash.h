#pragma once
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Camera/States/ICameraState.h"
#include "Application/Src/Object/Camera/CameraPathManager.h"

class CameraStateDash : public ICameraState
{
public:
	void Initialize() override;

	void Update() override;

private:
	void UpdateAnimationTime();

	void UpdateCameraTransformFromKeyFrame(const CameraKeyFrame& keyFrame);

	void CheckAnimationCompletion();

private:
	//カメラパス
	CameraPath cameraPath_{};

	//アニメーションの時間
	float animationTime_ = 0.0f;
};

