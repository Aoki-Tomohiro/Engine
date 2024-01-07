#include "Boss.h"
#include "Engine/Utilities/RandomGenerator.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Framework/GameObjectManager.h"
#include "Project/Object/Player/Player.h"
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

	////攻撃が当たったら揺らす
	//if (isHit_) 
	//{
	//	if (++shakeTimer_ >= kShakeTime)
	//	{
	//		isHit_ = false;
	//		shakeTimer_ = 0;
	//	}
	//	worldTransformAnimation_.translation_ += knockBackVelocity_;
	//	worldTransformAnimation_.translation_.x += RandomGenerator::GetRandomFloat(-0.1f, 0.1f);
	//	worldTransformAnimation_.translation_.z += RandomGenerator::GetRandomFloat(-0.1f, 0.1f);
	//	worldTransformAnimation_.UpdateMatrixFromEuler();
	//}
	//else
	//{
	//	worldTransformAnimation_ = worldTransform_;
	//}

	//ノックバックの処理
	if (isKnockBack_)
	{
		if (++knockBackTimer_ >= knockBackTime_)
		{
			isKnockBack_ = false;
		}

		worldTransform_.translation_ += knockBackVelocity_;
	}

	//移動限界座標
	const float kMoveLimitX = 47;
	const float kMoveLimitZ = 47;
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.z = max(worldTransform_.translation_.z, -kMoveLimitZ);
	worldTransform_.translation_.z = min(worldTransform_.translation_.z, +kMoveLimitZ);

	//基底クラスの更新
	IGameObject::Update();
}

void Boss::Draw(const Camera& camera) 
{
	//if (isHit_)
	//{
	//	model_->Draw(worldTransformAnimation_, camera);
	//}
	//else 
	{
		//基底クラスの描画
		IGameObject::Draw(camera);
	}
}

void Boss::OnCollision(Collider* collider) 
{
	if (collider->GetCollisionAttribute() == kCollisionAttributeWeapon)
	{
		onCollision_ = true;

		if (onCollision_ != preOnCollision_)
		{
			//ヒットフラグを立てる
			isHit_ = true;

			//ノックバックの速度を決める
			const Player* player = dynamic_cast<const Player*>(GameObjectManager::GetInstance()->GetGameObject("Player"));
			knockBackVelocity_ = player->GetVelocity();
			if (player->GetComboIndex() == 3)
			{
				Vector3 sub = worldTransform_.translation_ - player->GetTranslation();
				sub = Mathf::Normalize(sub);
				sub.y = 0.0f;
				const float kKnockBackSpeed = 2.0f;
				knockBackVelocity_ = sub * kKnockBackSpeed;
			}

			//ノックバックのフラグを立てる
			isKnockBack_ = true;

			//ノックバックのタイマーを設定
			knockBackTimer_ = 0;
			knockBackTime_ = player->GetAttackTime() - player->GetAttackParameter();

			//アニメーションのワールドトランスフォームの更新
			worldTransformAnimation_ = worldTransform_;
			worldTransformAnimation_.UpdateMatrixFromEuler();
		}
	}
}

const Vector3 Boss::GetWorldPosition() const
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld_.m[3][0];
	pos.y = worldTransform_.matWorld_.m[3][1];
	pos.z = worldTransform_.matWorld_.m[3][2];
	return pos;
}