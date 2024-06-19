#include "Player.h"
#include "Engine/Base/ImGuiManager.h"

void Player::Initialize()
{
	GameObject::Initialize();

	//トランスフォームの追加
	transformComponent_ = AddComponent<TransformComponent>();
	transformComponent_->translation_ = { 0.0f,3.0f,0.0f };
	transformComponent_->scale_ = { 3.0f,3.0f,3.0f };

	//モデルの追加
	model_ = ModelManager::CreateFromModelFile("MonsterBall", Opaque);
	modelComponent_ = AddComponent<ModelComponent>();
	modelComponent_->SetModel(model_);
	modelComponent_->SetTransformComponent(transformComponent_);

	//コライダーの追加
	SphereCollider* collider = AddComponent<SphereCollider>();
	collider->SetTransformComponent(transformComponent_);
}

void Player::Update()
{
	GameObject::Update();

	ImGui::Begin("Player");
	ImGui::DragFloat3("Translation", &transformComponent_->translation_.x, 0.1f);
	ImGui::DragFloat3("Rotation", &transformComponent_->rotation_.x, 0.01f);
	ImGui::DragFloat3("Scale", &transformComponent_->scale_.x, 0.01f);
	ImGui::End();
}

void Player::Draw(const Camera& camera)
{
	GameObject::Draw(camera);
}

void Player::OnCollision(GameObject* collider)
{
	ImGui::Begin("Player");
	ImGui::Text("OnCollision");
	ImGui::End();
}

void Player::OnCollisionEnter(GameObject* collider)
{
	ImGui::Begin("Player");
	ImGui::Text("OnCollisionEnter");
	ImGui::End();
}

void Player::OnCollisionExit(GameObject* collider)
{
	ImGui::Begin("Player");
	ImGui::Text("OnCollisionExit");
	ImGui::End();
}