#pragma once
#include "Component.h"
#include "Engine/3D/Transform/WorldTransform.h"

class TransformComponent : public Component
{
public:
	void Initialize() override;

	void Update() override;

	RotationType rotationType_;

	Vector3 translation_{};

	Vector3 rotation_{};

	Vector3 scale_{};

	Quaternion quaternion_{};
};

