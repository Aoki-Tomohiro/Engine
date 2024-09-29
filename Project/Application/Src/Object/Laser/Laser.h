#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Components/Collision/OBBCollider.h"
#include "Engine/Components/Collision/CollisionAttributeManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GameTimer.h"

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

	void Reset() override;

	void SetPosition(const Vector3& position) { transform_->worldTransform_.translation_ = position; };

	void SetRotation(const Quaternion& quaternion) { transform_->worldTransform_.quaternion_ = quaternion; };

	void SetTargetScale(const Vector3& targetScale) { targetScale_ = targetScale; };

	void SetLifeTime(const float lifeTime) { lifeTime_ = lifeTime; };

	void SetEasingSpeed(const float easingSpeed) { easingSpeed_ = easingSpeed; };

	const float GetDamage() const { return damage_; };

	void SetDamage(const float damage) { damage_ = damage; }

private:
	void InitializeTransform();

	void InitializeModel();

	void InitializeCollider();

	void UpdateLaserFading();

	void BeginLaserFading();

	void FadeOutLaser();

	void UpdateCollider();

private:
	//トランスフォーム
	TransformComponent* transform_ = nullptr;

	//モデル
	ModelComponent* model_ = nullptr;

	//コライダー
	OBBCollider* collider_ = nullptr;

	//目標スケール
	Vector3 targetScale_ = { 1.0f,1.0f,50.0f };

	//レーザーが徐々に消えているフラグ
	bool isLaserFading_ = false;

	//寿命
	float lifeTime_ = 4.0f;

	//死亡タイマー
	float laserFadingTimer_ = 0.0f;

	//イージング係数
	float easingParameter_ = 0.0f;

	//イージングスピード
	float easingSpeed_ = 1.0f;

	//ダメージ
	float damage_ = 1.0f;
};

