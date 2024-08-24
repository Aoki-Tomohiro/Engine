#include "Player.h"
#include "Engine/Components/Transform/TransformComponent.h"

void Player::Initialize()
{
	//基底クラスの初期化
	GameObject::Initialize();
}

void Player::Update()
{
	//基底クラスの更新
	GameObject::Update();

	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	ImGui::Begin("Player");
	ImGui::DragFloat3("Translate", &transformComponent->worldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("Rotation", &transformComponent->worldTransform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("Scale", &transformComponent->worldTransform_.scale_.x, 0.01f);
	ImGui::End();
}

void Player::Draw(const Camera& camera)
{
	//基底クラスの描画
	GameObject::Draw(camera);
}

void Player::OnCollision(GameObject* other)
{
}

void Player::OnCollisionEnter(GameObject* other)
{
}

void Player::OnCollisionExit(GameObject* other)
{
}