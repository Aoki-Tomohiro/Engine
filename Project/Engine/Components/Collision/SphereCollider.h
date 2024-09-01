#pragma once
#include "Collider.h"
#include "Engine/Math/Vector3.h"

class SphereCollider : public Collider
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	const Vector3& GetWorldCenter() const { return worldCenter_; };

	void SetWorldCenter(const Vector3& worldCenter)
	{
		worldCenter_ = worldCenter;
		isWorldCenterSet_ = true;
	};

	void ClearWorldCenter() { isWorldCenterSet_ = false; };

	const Vector3& GetCenter() const { return center_; };

	void SetCenter(const Vector3& center) { center_ = center; };

	const float GetRadius() const { return radius_; };
	 
	void SetRadius(const float radius) { radius_ = radius; };

private:
	Vector3 worldCenter_{};

	Vector3 center_{};

	float radius_ = 1.0f;

	bool isWorldCenterSet_ = false;
};

