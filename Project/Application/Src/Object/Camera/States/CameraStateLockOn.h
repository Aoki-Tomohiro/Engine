#pragma once
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/Camera/States/ICameraState.h"

class Player;

class CameraStateLockon : public ICameraState
{
public:
	void Initialize() override;

	void Update() override;

private:
	void UpdateCameraRotation();

	float CalculateRotationAngle() const;

	bool IsWithinRotationRange(const float angle) const;

	Quaternion CalculateNewRotation() const;

	void ManageCameraStateTransition();

private:
	//前のフレームの方向ベクトル
	Vector3 previousDirection_{};

	//現在の方向ベクトル
	Vector3 currentDirection_{};
};

