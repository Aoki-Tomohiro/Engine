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

	void DrawUI() override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	void SetParent(const WorldTransform* worldTransform);

	void SetisParryable(const bool isParryable) { isParryable_ = isParryable; };

	const bool GetIsParrySuccessful() const { return isParrySuccessful_; };

	void SetIsAttack(const bool isAttack) { isAttack_ = isAttack; };

	const bool GetIsAttack() const { return isAttack_; };

	const bool GetIsHit() const { return isHit_; };

private:
	//Particle
	ParticleSystem* particleSystem_ = nullptr;

	//Collider
	Vector3 collisionOffset_{ 0.0f,1.2f,0.0f };
	Vector3 size_{ 0.2f,3.5f,0.4f };

	//攻撃フラグ
	bool isAttack_ = false;

	//ヒットフラグ
	bool isHit_ = false;

	//パリィ出来る状態か
	bool isParryable_ = false;

	//パリィに成功したか
	bool isParrySuccessful_ = false;

	//デバッグ用のフラグ
	bool isDebug_ = false;
};

