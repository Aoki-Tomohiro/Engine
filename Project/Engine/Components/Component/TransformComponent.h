#pragma once
#include "Component.h"
#include "Engine/3D/Transform/WorldTransform.h"

class TransformComponent : public Component
{
public:
	void Initialize() override;

	void Update() override;

	const Vector3 GetWorldPosition();

	WorldTransform worldTransform_{};
};

