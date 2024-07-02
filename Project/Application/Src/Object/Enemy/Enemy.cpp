#include "Enemy.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"

void Enemy::Initialize()
{
	//GameObjectの初期化
	GameObject::Initialize();
}

void Enemy::Update()
{
	//Colliderの更新
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
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

}

void Enemy::OnCollisionExit(GameObject* gameObject)
{

}