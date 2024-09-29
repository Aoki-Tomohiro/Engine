#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Components/Collision/OBBCollider.h"
#include "Engine/Components/Collision/CollisionAttributeManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GameTimer.h"

class Pillar : public GameObject
{
public:
	enum class PillarState
	{
		kInactive,   // 非アクティブ
		kSpawning,   // 出現中
		kDespawning, // 消失中
	};

	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void DrawUI() override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	void Reset() override;

	void SetInactiveDuration(const float inactiveDuration) { inactiveDuration_ = inactiveDuration; };

	void SetPreSpawnPosition(const Vector3& preSpawnPosition) { preSpawnPosition_ = preSpawnPosition; };

	void SetSpawnedPosition(const Vector3& spawnedPosition) { spawnedPosition_ = spawnedPosition; };

	const float GetDamage() const { return damage_; };

private:
	void InitializeTransform();

	void InitializeModel();

	void InitializeCollider();

	void UpdatePillarState();

	void InactiveUpdate();

	void SpawnUpdate();

	void DespawnUpdate();

	void UpdateCollider();

private:
	//トランスフォーム
	TransformComponent* transform_ = nullptr;

	//モデル
	ModelComponent* model_ = nullptr;

	//コライダー
	OBBCollider* collider_ = nullptr;

	//出現前の座標
	Vector3 preSpawnPosition_{};

	//出現後の座標
	Vector3 spawnedPosition_{};

	//状態
	PillarState pillarState_ = PillarState::kInactive;

	//非アクティブの時間とタイマー
	float inactiveDuration_ = 1.0f;
	float inactiveTimer_ = 0.0f;

	//完全に出現するまでの時間とタイマー
	float spawnDuration_ = 0.2f;
	float spawnTimer_ = 0.0f;

	//消失するまでの時間とタイマー
	float despawnDuration_ = 1.0f;
	float despawnTimer_ = 0.0f;

	//透明度
	float alpha_ = 1.0f;

	//ダメージ
	float damage_ = 6.0f;
};

