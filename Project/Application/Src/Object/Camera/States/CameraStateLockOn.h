#pragma once
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/Camera/States/ICameraState.h"

class CameraStateLockon : public ICameraState
{
public:
	void Initialize() override;

	void Update() override;

private:
	void UpdateCameraRotation();
};

