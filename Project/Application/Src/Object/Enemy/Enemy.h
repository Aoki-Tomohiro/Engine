#pragma once
#include "Engine/Framework/Object/GameObject.h"

class Enemy : public GameObject
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void OnCollision(GameObject* collider) override;

	void OnCollisionEnter(GameObject* collider) override;

	void OnCollisionExit(GameObject* collider) override;
};
