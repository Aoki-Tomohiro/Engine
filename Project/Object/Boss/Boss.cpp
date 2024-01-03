#include "Boss.h"
#include "Engine/Utilities/RandomGenerator.h"
#include "Project/Object/Player/Weapon.h"
#include "Engine/Components/CollisionConfig.h"

void Boss::Initialize() 
{
	//基底クラスの初期化
	IGameObject::Initialize();
	worldTransform_.translation_.y = 3.0f;
	worldTransform_.scale_ = { 3.0f,3.0f,3.0f };

	//衝突判定用のワールドトランスフォームの初期化
	worldTransformAnimation_.Initialize();
	worldTransformAnimation_ = worldTransform_;

	//衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionMaskEnemy);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
	AABB aabb{ 
		.min{-worldTransform_.scale_.x,-worldTransform_.scale_.y,-worldTransform_.scale_.z}, 
		.max{worldTransform_.scale_.x,worldTransform_.scale_.y,worldTransform_.scale_.z} };
	SetAABB(aabb);
}

void Boss::Update()
{
	//前のフレームの当たり判定のフラグを取得
	preOnCollision_ = onCollision_;
	onCollision_ = false;

	//攻撃が当たったら揺らす
	if (isHit_) 
	{
		if (++shakeTimer_ >= kShakeTime)
		{
			isHit_ = false;
			shakeTimer_ = 0;
		}
		worldTransformAnimation_.translation_.x += RandomGenerator::GetRandomFloat(-0.1f, 0.1f);
		worldTransformAnimation_.translation_.z += RandomGenerator::GetRandomFloat(-0.1f, 0.1f);
		worldTransformAnimation_.UpdateMatrixFromEuler();
	}
	else
	{
		worldTransformAnimation_ = worldTransform_;
	}

	//基底クラスの更新
	IGameObject::Update();
}

void Boss::Draw(const Camera& camera) 
{
	if (isHit_)
	{
		model_->Draw(worldTransformAnimation_, camera);
	}
	else 
	{
		//基底クラスの描画
		IGameObject::Draw(camera);
	}
}

void Boss::OnCollision(Collider* collider) 
{
	onCollision_ = true;

	if (collider->GetCollisionAttribute() == kCollisionAttributeWeapon)
	{
		if (onCollision_ != preOnCollision_)
		{
			isHit_ = true;
			worldTransformAnimation_ = worldTransform_;
			worldTransformAnimation_.UpdateMatrixFromEuler();
		}
	}
}

Vector3 Boss::GetWorldPosition()
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld_.m[3][0];
	pos.y = worldTransform_.matWorld_.m[3][1];
	pos.z = worldTransform_.matWorld_.m[3][2];
	return pos;
}