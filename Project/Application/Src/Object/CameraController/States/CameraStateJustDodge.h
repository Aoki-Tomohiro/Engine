#pragma once
#include "ICameraState.h"
#include "Engine/Math/MathFunction.h"

class CameraStateJustDodge : public ICameraState
{
public:
	void Initialize() override;

	void Update() override;
};
