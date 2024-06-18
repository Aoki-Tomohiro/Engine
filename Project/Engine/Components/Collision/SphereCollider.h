#pragma once
#include "Collider.h"
#include "Engine/Math/Vector3.h"

class SphereCollider : public Collider
{
public:
	void Initialize() override;

	void Update() override;

	const Vector3& GetCenter() const { return center_; };

	void SetCenter(const Vector3& center) { center_ = center; };

	const float GetRadius() const { return radius_; };
	 
	void SetRadius(const float radius) { radius_ = radius; };

private:
	Vector3 center_{};

	float radius_ = 1.0f;
};

