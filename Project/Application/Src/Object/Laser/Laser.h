#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Math/MathFunction.h"

class Laser : public GameObject
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void DrawUI() override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	void SetTargetScale(const Vector3& targetScale) { targetScale_ = targetScale; };

	void SetLifeTime(const float lifeTime) { lifeTime_ = lifeTime; };

	void SetEasingSpeed(const float easingSpeed) { easingSpeed_ = easingSpeed; };

private:
	Vector3 targetScale_ = { 1.0f,1.0f,50.0f };

	bool isDead_ = false;

	float lifeTime_ = 4.0f;

	float deathTimer_ = 0.0f;

	float easingParameter_ = 0.0f;

	float easingSpeed_ = 1.0f;
};

