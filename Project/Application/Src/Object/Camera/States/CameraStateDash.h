#pragma once
#include "Application/Src/Object/Camera/States/ICameraState.h"

class CameraStateDash : public ICameraState
{
public:
	void Initialize() override;

	void Update() override;

private:
	//開始時のFov
	float startFov_ = 0.0f;

	//イージングの時間
	float easingTimer_ = 0.0f;
};

