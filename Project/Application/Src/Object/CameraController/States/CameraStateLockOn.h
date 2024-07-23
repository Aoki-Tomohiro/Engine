#pragma once
#include "ICameraState.h"
#include "Engine/Math/MathFunction.h"

class CameraStateLockOn : public ICameraState
{
public:
	void Initialize() override;

	void Update() override;
};

