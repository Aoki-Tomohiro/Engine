#pragma once
#include "Engine/3D/Model/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/Input/Input.h"

class LockOn;

class FollowCamera
{
public:
	void Initialize();

	void Update();

	void SetTarget(const WorldTransform* target);

	const Camera& GetCamera() { return camera_; };

	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; };

private:
	Vector3 Offset();

	void Reset();

private:
	Input* input_ = nullptr;

	Camera camera_{};

	const WorldTransform* target_{};

	Vector3 interTarget_{};

	float destinationAngleX_ = 0.0f;
	float destinationAngleY_ = 0.0f;

	const LockOn* lockOn_ = nullptr;
};

