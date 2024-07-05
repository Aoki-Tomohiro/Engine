#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Components/Particle/ParticleManager.h"
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

	const bool GetIsHit() const { return isHit_; };

private:
	//Particle
	ParticleSystem* particleSystem_ = nullptr;

	//Collider
	Vector3 collisionOffset_{ 0.0f,1.2f,0.0f };
	Vector3 size_{ 0.2f,3.36f,0.28f };

	//ヒットフラグ
	bool isHit_ = false;
};

