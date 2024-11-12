#pragma once
#include "Engine/Components/Base/Component.h"
#include "Engine/3D/Transform/WorldTransform.h"

class TransformComponent : public Component
{
public:
	void Initialize() override;

	void Update() override;

	const Vector3 GetWorldPosition() const;

	WorldTransform worldTransform_{};
};

