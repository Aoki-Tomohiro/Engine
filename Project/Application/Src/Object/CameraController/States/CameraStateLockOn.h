#pragma once
#include "ICameraState.h"
#include "Engine/Math/MathFunction.h"

class CameraStateLockOn : public ICameraState
{
public:
	void Initialize() override;

	void Update() override;

private:
	Vector3 Offset();

private:
	//追従対象からのオフセット
	Vector3 offset_{ 0.0f, 2.0f, -20.0f };
};

