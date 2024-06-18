#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Components/Collision/OBBCollider.h"

class Enemy : public GameObject
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void OnCollision(GameObject* collider) override;

	void OnCollisionEnter(GameObject* collider) override;

	void OnCollisionExit(GameObject* collider) override;

private:
	Model* model_ = nullptr;

	ModelComponent* modelComponent_ = nullptr;

	TransformComponent* transformComponent_ = nullptr;
};

