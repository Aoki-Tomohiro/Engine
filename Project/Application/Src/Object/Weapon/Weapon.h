#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Math/MathFunction.h"

class Weapon : public GameObject
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	void SetParent(const TransformComponent* transformComponent);

private:
	Vector3 colliderSize_ = { 0.2f,2.55f,0.32f };
	Vector3 offset_ = { 0.0f,2.0f,0.0f };
};

