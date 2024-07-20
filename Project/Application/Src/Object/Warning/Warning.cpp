#include "Warning.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Collision/OBBCollider.h"

void Warning::Initialize()
{
	//GameObjectの初期化
	GameObject::Initialize();
}

void Warning::Update()
{
	//Colliderの更新
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	OBBCollider* collider = GetComponent<OBBCollider>();
	collider->SetOrientations(
		{ transformComponent->worldTransform_.matWorld_.m[0][0], transformComponent->worldTransform_.matWorld_.m[0][1], transformComponent->worldTransform_.matWorld_.m[0][2] },
		{ transformComponent->worldTransform_.matWorld_.m[1][0], transformComponent->worldTransform_.matWorld_.m[1][1], transformComponent->worldTransform_.matWorld_.m[1][2] },
		{ transformComponent->worldTransform_.matWorld_.m[2][0], transformComponent->worldTransform_.matWorld_.m[2][1], transformComponent->worldTransform_.matWorld_.m[2][2] }
	);

	//Gameobjectの更新
	GameObject::Update();

	//ImGui
	ImGui::Begin("Warning");
	ImGui::DragFloat3("Scale", &transformComponent->worldTransform_.scale_.x, 0.01f);
	ImGui::DragFloat3("Translation", &transformComponent->worldTransform_.translation_.x, 0.01f);
	ImGui::End();
}

void Warning::Draw(const Camera& camera)
{
	//GameObjectの描画
	GameObject::Draw(camera);
}

void Warning::DrawUI()
{

}

void Warning::OnCollision(GameObject* gameObject)
{
}

void Warning::OnCollisionEnter(GameObject* gameObject)
{
}

void Warning::OnCollisionExit(GameObject* gameObject)
{
}