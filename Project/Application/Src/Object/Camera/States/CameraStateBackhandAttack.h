#pragma once
#include "Application/Src/Object/Camera/States/ICameraState.h"
#include "Application/Src/Object/Camera/CameraPathManager.h"

class CameraStateBackhandAttack : public ICameraState
{
public:
	void Initialize() override;

	void Update() override;

private:
	CameraPath cameraPath_{};
};

