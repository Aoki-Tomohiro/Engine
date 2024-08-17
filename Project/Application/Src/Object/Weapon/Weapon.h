#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GlobalVariables.h"

class Weapon : public GameObject
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	const bool GetIsAttack() const { return isAttack_; };

	void SetIsAttack(const bool isAttack) { isAttack_ = isAttack; };

	const bool GetIsHit() const { return isHit_; };

private:
	void UpdateCollider();

	void ApplyGlobalVariables();

private:
	//Audio
	Audio* audio_ = nullptr;

	//パーティクル
	ParticleSystem* particleSystem_ = nullptr;

	//Colliderのオフセット
	Vector3 colliderOffset_{ 0.0f, 0.0f, -3.0f };

	//Colliderのサイズ
	Vector3 colliderSize_{ 2.0f, 2.0f, 2.0f };

	//攻撃フラグ
	bool isAttack_ = false;

	//ヒットフラグ
	bool isHit_ = false;

	//デバッグフラグ
	bool isDebug_ = false;
};

