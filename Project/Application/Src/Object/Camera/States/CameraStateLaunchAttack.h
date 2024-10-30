#pragma once
#include "Application/Src/Object/Camera/States/ICameraState.h"
#include "Application/Src/Object/Editors/CameraAnimationEditor/CameraPath.h"

class Player;

class CameraStateLaunchAttack : public ICameraState
{
public:
	void Initialize() override;

	void Update() override;

private:
	void UpdateAnimationTime();

	void UpdateCameraTransform(const CameraKeyFrame& keyFrame, Player* player);

	void CheckAnimationCompletion();

private:
	//カメラパス
	CameraPath cameraPath_{};

	//アニメーションの時間
	float animationTime = 0.0f;
};

