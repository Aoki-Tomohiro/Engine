#include "Weapon.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Components/CollisionConfig.h"

void Weapon::Initialize() 
{
	//基底クラスの初期化
	IGameObject::Initialize();
	worldTransform_.translation_.y = 0.8f;

	//衝突判定用のワールドトランスフォームの初期化
	worldTransformCollision_.Initialize();

	//パーティクルシステムの初期化
	particleSystem_ = ParticleManager::Create("Weapon");

	//衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeWeapon);
	SetCollisionMask(kCollisionMaskWeapon);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
}

void Weapon::Update() 
{
	//親子付けされていなかったらする
	if (worldTransform_.parent_ == nullptr) 
	{
		worldTransform_.parent_ = playerWorldTransform_;
	}

	//前のフレームの当たり判定のフラグを取得
	preOnCollision_ = onCollision_;
	onCollision_ = false;
	isHit_ = false;

	//当たり判定の位置を決める
	Vector3 offset{ 0.0f,0.0f,2.0f };
	offset = Mathf::TransformNormal(offset, playerWorldTransform_->matWorld_);
	worldTransformCollision_.translation_ = playerWorldTransform_->translation_;
	worldTransformCollision_.quaternion_ = playerWorldTransform_->quaternion_;
	worldTransformCollision_.translation_ += offset;

	//ワールドトランスフォームの更新
	worldTransformCollision_.UpdateMatrixFromQuaternion();

	//基底クラスの更新
	IGameObject::Update();
}

void Weapon::UpdateParticle()
{
	//パーティクルシステムの更新
	particleSystem_->Update();
}

void Weapon::Draw(const Camera& camera) 
{
	//基底クラスの描画
	IGameObject::Draw(camera);
}

void Weapon::DrawParticle(const Camera& camera)
{
	//パーティクルシステムの描画
	particleSystem_->Draw(camera);
}

void Weapon::OnCollision(Collider* collider)
{
	onCollision_ = true;

	if (onCollision_ != preOnCollision_) 
	{
		//ヒットフラグを立てる
		isHit_ = true;
		ParticleEmitter* emitter = ParticleEmitterBuilder()
			.SetDeleteTime(1.0f)
			.SetEmitterName("Hit")
			.SetPopArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetPopAzimuth(0.0f, 360.0f)
			.SetPopColor({ 1.0f,1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f })
			.SetPopCount(200)
			.SetPopElevation(-90.0f, 90.0f)
			.SetPopFrequency(2.0f)
			.SetPopLifeTime(0.2f, 0.2f)
			.SetPopRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetPopScale({ 0.2f,0.2f,0.2f }, { 0.2f,0.2f,0.2f })
			.SetPopVelocity({ 0.2f,0.2f,0.2f }, { 0.2f,0.2f,0.2f })
			.SetTranslation({ worldTransform_.translation_.x,worldTransform_.translation_.y + 1.0f,worldTransform_.translation_.z })
			.Build();
		//エミッターを追加
		particleSystem_->AddParticleEmitter(emitter);
	}
}

Vector3 Weapon::GetWorldPosition() 
{
	Vector3 pos{};
	pos.x = worldTransformCollision_.matWorld_.m[3][0];
	pos.y = worldTransformCollision_.matWorld_.m[3][1];
	pos.z = worldTransformCollision_.matWorld_.m[3][2];
	return pos;
}

void Weapon::UpdateImGui()
{
	ImGui::Begin("Weapon");
	ImGui::DragFloat3("Translation", &worldTransformCollision_.translation_.x);
	ImGui::End();
}