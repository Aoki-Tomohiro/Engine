#include "Weapon.h"
#include "Engine/Base/ImGuiManager.h"

void Weapon::Initialize()
{
	//GameObjectの初期化
	GameObject::Initialize();
}

void Weapon::Update()
{
	//Gameobjectの更新
	GameObject::Update();

	//ImGui
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	ImGui::Begin("Weapon");
	ImGui::DragFloat3("Translate", &transformComponent->worldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("Rotation", &transformComponent->worldTransform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("Scale", &transformComponent->worldTransform_.scale_.x, 0.01f);
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