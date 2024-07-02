#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Base/ImGuiManager.h"

class Enemy : public GameObject
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

private:
	//Collider
	Vector3 colliderOffset_{ 0.0f,4.0f,0.0f };
};

