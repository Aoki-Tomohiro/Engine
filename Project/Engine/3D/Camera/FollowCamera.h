#pragma once
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Transform/WorldTransform.h"
#include "Engine/Components/Input/Input.h"

class FollowCamera 
{
public:
	void Initialize();

	void Update();

	void SetTarget(const WorldTransform* target);

	const Camera& GetCamera() const { return camera_; };

private:
	Vector3 Offset();

	void Reset();

private:
	//カメラ
	Camera camera_{};

	//追従対象
	const WorldTransform* target_ = nullptr;

	//追従対象の残像座標
	Vector3 interTarget_{};

	//目標角度
	float destinationAngleX_ = 0.0f;
	float destinationAngleY_ = 0.0f;
};

