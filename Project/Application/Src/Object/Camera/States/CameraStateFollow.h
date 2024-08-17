#pragma once
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/Camera/States/ICameraState.h"

class CameraStateFollow : public ICameraState
{
public:
	void Initialize() override;

	void Update() override;

private:
	void Reset();

private:
	//Input
	Input* input_ = nullptr;

	//累積用のQuaternion
	Quaternion quaternionX = { 0.0f,0.0f,0.0f,1.0f };
	Quaternion quaternionY = { 0.0f,0.0f,0.0f,1.0f };
};

