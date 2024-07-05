#include "Enemy.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Engine/Math/MathFunction.h"
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

	//Colliderの更新
	AABBCollider* collider = GetComponent<AABBCollider>();
	collider->SetCenter(transformComponent->GetWorldPosition() + colliderOffset_);

	//GameObjectの更新
	GameObject::Update();

	//ImGui
	ImGui::Begin("Enemy");
	ImGui::DragFloat3("ColliderOffset", &colliderOffset_.x, 0.1f);
	ImGui::End();
}

void Enemy::Draw(const Camera& camera)
{
	//GameObjectの描画
	GameObject::Draw(camera);
}

void Enemy::OnCollision(GameObject* gameObject)
{

}

void Enemy::OnCollisionEnter(GameObject* gameObject)
{
	if (Weapon* weapon = dynamic_cast<Weapon*>(gameObject))
	{
		ImGui::Begin("Enemy");
		ImGui::Text("Hit");
		ImGui::End();
	}
}

void Enemy::OnCollisionExit(GameObject* gameObject)
{

}

const bool Enemy::GetIsAttack() const
{
	if (auto state = dynamic_cast<EnemyStateTackle*>(state_.get()))
	{
		return state->GetIsAttack();
	}
	return false;
}

void Enemy::ChangeState(IEnemyState* state)
{
	state->SetEnemy(this);
	state->Initialize();
	state_.reset(state);
}