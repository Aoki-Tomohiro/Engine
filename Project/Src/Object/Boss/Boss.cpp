#include "Boss.h"

Boss::~Boss()
{

}

void Boss::Initialize()
{
	worldTransform_.Initialize();
	worldTransform_.translation_.y = 3.0f;
	worldTransform_.scale_ = { 3.0f,3.0f,3.0f };

	//体力バーのスプライトの生成
	spriteHpBar_.reset(Sprite::Create("HpBar.png", { 720.0f,32.0f }));
	spriteHpBar_->SetColor({ 1.0f, 0.1f, 0.0f, 1.0f });

	//体力バーのフレームのスプライトの生成
	spriteHpBarFrame_.reset(Sprite::Create("HpBarFrame.png", { 719.0f,31.0f }));
	spriteHpBarFrame_->SetColor({ 1.0f, 0.1f, 0.0f, 1.0f });

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
	//ワールドトランスフォームの更新
	worldTransform_.UpdateMatrixFromQuaternion();

	//HPバーの処理
	hpBarSize_ = { (hp_ / kMaxHP) * 480.0f,16.0f };
	spriteHpBar_->SetSize(hpBarSize_);
}

void Boss::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera);
}

void Boss::DrawUI()
{
	spriteHpBar_->Draw();
	spriteHpBarFrame_->Draw();
}

void Boss::OnCollision(Collider* collider)
{

}

const Vector3 Boss::GetWorldPosition() const
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld_.m[3][0];
	pos.y = worldTransform_.matWorld_.m[3][1];
	pos.z = worldTransform_.matWorld_.m[3][2];
	return pos;
}