#pragma once
#include "Engine/Framework/Object/GameObject.h"

class MagicProjectile : public GameObject
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void DrawUI() override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; };

	void SetisEnhanced(const bool isEnhanced) { isEnhanced_ = isEnhanced; };

private:
	//速度
	Vector3 velocity_{};

	//強化状態かどうか
	bool isEnhanced_ = false;
};

