#pragma once
#include "Collider.h"
#include "Engine/Math/Vector3.h"

class AABBCollider : public Collider
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	const Vector3& GetCenter() const { return center_; };

	void SetCenter(const Vector3& center) { center_ = center; };

	const Vector3& GetMin() const { return min_; };

	void SetMin(const Vector3& min) { min_ = min; };

	const Vector3& GetMax() const { return max_; };

	void SetMax(const Vector3& max) { max_ = max; };

private:
	Vector3 center_{};

	Vector3 min_{ -1.0f,-1.0f,-1.0f };

	Vector3 max_{ 1.0f,1.0f,1.0f };
};

