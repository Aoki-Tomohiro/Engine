#pragma once
#include "ICameraState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"

class CameraStateFollow : public ICameraState
{
public:
	void Initialize() override;

	void Update() override;

private:
	Vector3 Offset();

	void Reset();

private:
	//Input
	Input* input_ = nullptr;

	//追従対象の残像座標
	Vector3 interTarget_{};

	//追従対象からのオフセット
	Vector3 offset_{ 0.0f, 2.0f, -20.0f };

	float yaw_ = 0.0f;

	float pitch_ = 0.0f;
};

