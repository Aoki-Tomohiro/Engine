#pragma once
#include "Application/Src/Object/Camera/States/ICameraState.h"
#include "Application/Src/Object/Camera/CameraPathManager.h"

class Player;

class CameraStateFallingAttack : public ICameraState
{
public:
	void Initialize() override;

	void Update() override;

private:
	void UpdateAnimationTime(Player* player);

	void UpdateCameraTransform(const CameraKeyFrame& keyFrame, Player* player);

	void CheckAnimationCompletion(Player* player);

private:
	//カメラパス
	CameraPath cameraPath_{};

	//アニメーションの時間
	float animationTime_ = 0.0f;

	//アニメーションの速度
	float animationSpeed_ = 3.0f;
};

