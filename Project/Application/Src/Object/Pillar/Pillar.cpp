#include "Pillar.h"

void Pillar::Initialize()
{
	//トランスフォームの初期化
	transform_ = GetComponent<TransformComponent>();
	transform_->worldTransform_.rotationType_ = RotationType::Quaternion;

	//モデルの初期化
	model_ = AddComponent<ModelComponent>();
	model_->SetModel(ModelManager::CreateFromModelFile("Spear", Transparent));

	//コライダーの初期化
	collider_ = AddComponent<OBBCollider>();
	collider_->SetCollisionAttribute(CollisionAttributeManager::GetInstance()->GetAttribute("Pillar"));
	collider_->SetCollisionMask(CollisionAttributeManager::GetInstance()->GetMask("Pillar"));

	//状態の初期化
	ChangeState(new PillarStateInactive());

	//基底クラスの初期化
	GameObject::Initialize();
}

void Pillar::Update()
{
	//状態の更新
	state_->Update();

	//コライダーの更新
	UpdateCollider();

	//基底クラスの更新
	GameObject::Update();
}

void Pillar::ChangeState(IPillarState* newState)
{
	//新しい状態の設定
	newState->SetPillar(this);

	//現在の状態を新しい状態に更新
	state_.reset(newState);
}

void Pillar::UpdatePositionWithEasing(const float easingParameter)
{
	//イージングさせる
	transform_->worldTransform_.translation_ = preSpawnPosition_ + (spawnedPosition_ - preSpawnPosition_) * Mathf::EaseInCubic(easingParameter);
}

void Pillar::UpdateCollider()
{
	//コライダーの向きとサイズを更新
	collider_->SetOrientations(
		{ transform_->worldTransform_.matWorld_.m[0][0], transform_->worldTransform_.matWorld_.m[0][1], transform_->worldTransform_.matWorld_.m[0][2] },
		{ transform_->worldTransform_.matWorld_.m[1][0], transform_->worldTransform_.matWorld_.m[1][1], transform_->worldTransform_.matWorld_.m[1][2] },
		{ transform_->worldTransform_.matWorld_.m[2][0], transform_->worldTransform_.matWorld_.m[2][1], transform_->worldTransform_.matWorld_.m[2][2] }
	);
	collider_->SetSize(colliderSize_);
}