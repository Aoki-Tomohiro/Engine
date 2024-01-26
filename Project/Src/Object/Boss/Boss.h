#pragma once
#include "Engine/Framework/Object/IGameObject.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/2D/Sprite.h"

class Boss : public IGameObject, public Collider
{
public:
	~Boss();

	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void DrawUI() override;

	void OnCollision(Collider* collider) override;

	const Vector3 GetWorldPosition() const override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

private:
	//体力バー
	std::unique_ptr<Sprite> spriteHpBar_ = nullptr;
	std::unique_ptr<Sprite> spriteHpBarFrame_ = nullptr;

	//HP
	Vector2 hpBarSize_{ 480.0f,16.0f };
	const float kMaxHP = 800.0f;
	float hp_ = kMaxHP;
};

