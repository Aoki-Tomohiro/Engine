#include "Enemy.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Enemy/States/EnemyStateTackle.h"

void Enemy::Initialize()
{
	//GameObjectの初期化
	GameObject::Initialize();

	//Stateの初期化
	ChangeState(new EnemyStateIdle());
}

void Enemy::Update()
{
	//タイトルシーンにいる場合は移動処理をスキップ
	if (!isInTitleScene_)
	{
		//Stateの更新
		state_->Update();
	}

	//回転処理
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.quaternion_ = Mathf::Normalize(Mathf::Slerp(transformComponent->worldTransform_.quaternion_, destinationQuaternion_, 0.4f));

	//Colliderを設定
	AABBCollider* collider = GetComponent<AABBCollider>();
	collider->SetDebugDrawEnabled(isDebug_);

	//GameObjectの更新
	GameObject::Update();

	//ImGui
	ImGui::Begin("Enemy");
	ImGui::Checkbox("IsDebug", &isDebug_);
	ImGui::DragFloat("AnimationTime", &animationTime_, 0.01f);
	ImGui::End();
}

void Enemy::Draw(const Camera& camera)
{
	//GameObjectの描画
	GameObject::Draw(camera);
}

void Enemy::DrawUI()
{

}

void Enemy::OnCollision(GameObject* gameObject)
{

}

void Enemy::OnCollisionEnter(GameObject* gameObject)
{

}

void Enemy::OnCollisionExit(GameObject* gameObject)
{

}

void Enemy::ChangeState(IEnemyState* state)
{
	state->SetEnemy(this);
	state->Initialize();
	state_.reset(state);
}