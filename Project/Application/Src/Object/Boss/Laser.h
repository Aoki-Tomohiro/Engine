#pragma once
#include "Engine/Framework/Object/IGameObject.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/Math/MathFunction.h"

class Laser : public IGameObject
{
public:
	//レーザーの寿命
	static const uint32_t kLifeTime = 60 * 10;

	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	void OnCollision(Collider* collider) override;

	//const Vector3 GetWorldPosition() const override;

	//const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

	const bool GetIsDead() const { return isDead_; };

private:
	//モデル
	Model* model_ = nullptr;

	//ワールドトランスフォーム
	WorldTransform worldTransform_{};

	//死亡フラグ
	bool isDead_ = false;

	//タイマー
	uint32_t lifeTimer_ = 0;

	//OBB
	OBB obbSize{};

	//目標スケール
	Vector3 targetScale_{ 1.5f,1.5f,50.0f };
};

