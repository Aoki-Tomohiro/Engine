#include "Weapon.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Collision/OBBCollider.h"

void Weapon::Initialize()
{
	//GameObjectの初期化
	GameObject::Initialize();
}

void Weapon::Update()
{
	//Gameobjectの更新
	GameObject::Update();

	//Colliderの更新
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	OBBCollider* colliderComponent = GetComponent<OBBCollider>();
	colliderComponent->SetDebugDrawEnabled(true);
	colliderComponent->SetCenter(transformComponent->GetWorldPosition());
	colliderComponent->SetOrientations(
		{ transformComponent->worldTransform_.matWorld_.m[0][0], transformComponent->worldTransform_.matWorld_.m[0][1], transformComponent->worldTransform_.matWorld_.m[0][2] },
		{ transformComponent->worldTransform_.matWorld_.m[1][0], transformComponent->worldTransform_.matWorld_.m[1][1], transformComponent->worldTransform_.matWorld_.m[1][2] },
		{ transformComponent->worldTransform_.matWorld_.m[2][0], transformComponent->worldTransform_.matWorld_.m[2][1], transformComponent->worldTransform_.matWorld_.m[2][2] }
	);
	colliderComponent->SetSize(colliderSize_);

	//ImGui
	ImGui::Begin("Weapon");
	ImGui::DragFloat3("Translate", &transformComponent->worldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("Rotation", &transformComponent->worldTransform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("Scale", &transformComponent->worldTransform_.scale_.x, 0.01f);
	ImGui::DragFloat3("ColliderSize", &colliderSize_.x, 0.01f);
	ImGui::DragFloat3("Offset", &offset_.x, 0.01f);
	ImGui::End();
}

void Weapon::Draw(const Camera& camera)
{
	//GameObjectの描画
	GameObject::Draw(camera);
}

void Weapon::OnCollision(GameObject* gameObject)
{
}

void Weapon::OnCollisionEnter(GameObject* gameObject)
{
}

void Weapon::OnCollisionExit(GameObject* gameObject)
{
}

void Weapon::SetParent(const TransformComponent* parentTransformComponent)
{
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.parent_ = &parentTransformComponent->worldTransform_;
}