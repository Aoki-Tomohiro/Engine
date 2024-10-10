#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Components/Collision/OBBCollider.h"
#include "Engine/Components/Collision/CollisionAttributeManager.h"
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/Pillar/States/PillarStateInactive.h"

/// <summary>
/// 柱
/// </summary>
class Pillar : public GameObject
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 状態遷移
	/// </summary>
	/// <param name="newState">新しい状態</param>
	void ChangeState(IPillarState* newState);

	/// <summary>
	/// イージングによって座標を更新する
	/// </summary>
	/// <param name="easingParameter">イージング係数</param>
	void UpdatePositionWithEasing(const float easingParameter);

	//出現するまでの時間を取得・設定
	const float GetInactiveDuration() const { return inactiveDuration_; };
	void SetInactiveDuration(const float inactiveDuration) { inactiveDuration_ = inactiveDuration; };

	//出現前の座標を設定
	void SetPreSpawnPosition(const Vector3& preSpawnPosition) { preSpawnPosition_ = preSpawnPosition; };

	//出現後の座標を設定
	void SetSpawnedPosition(const Vector3& spawnedPosition) { spawnedPosition_ = spawnedPosition; };

	//モデルコンポーネントを取得
	ModelComponent* GetModelComponent() const { return model_; };

	//コライダーを取得
	OBBCollider* GetCollider() const { return collider_; };

	//ダメージを取得
	const float GetDamage() const { return damage_; };

private:
	/// <summary>
	/// コライダーの更新
	/// </summary>
	void UpdateCollider();

private:
	//状態
	std::unique_ptr<IPillarState> state_ = nullptr;

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

	//コライダーのサイズ
	Vector3 colliderSize_ = { 2.0f,14.0f,2.0f };

	//非アクティブの時間
	float inactiveDuration_ = 1.0f;

	//ダメージ
	float damage_ = 6.0f;
};

