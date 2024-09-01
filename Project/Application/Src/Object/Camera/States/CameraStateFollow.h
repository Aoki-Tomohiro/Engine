#pragma once
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/Camera/States/ICameraState.h"
#include <numbers>

class CameraStateFollow : public ICameraState
{
public:
	void Initialize() override;

	void Update() override;

private:
	void UpdateCameraRotation();

	void ClampVerticalRotation(Vector3& input, const Vector3& direction);

	void ApplyCameraRotation(const Vector3& input);

	Vector3 GetRightStickInput() const;

private:
	//Input
	Input* input_ = nullptr;

	//X軸のQuaternion
	Quaternion quaternionX = Mathf::IdentityQuaternion();

	//Y軸のQuaternion
	Quaternion quaternionY = Mathf::IdentityQuaternion();
};

