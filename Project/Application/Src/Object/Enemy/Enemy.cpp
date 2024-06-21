#include "Enemy.h"
#include "Engine/Base/ImGuiManager.h"

void Enemy::Initialize()
{
	GameObject::Initialize();

	//トランスフォームの追加
	transformComponent_ = AddComponent<TransformComponent>();
	transformComponent_->rotation_ = { 0.0f,-1.57f,0.0f };
	transformComponent_->scale_ = { 3.0f,3.0f,3.0f };

	//モデルの追加
	model_ = ModelManager::CreateFromModelFile("MonsterBall", Opaque);
	modelComponent_ = AddComponent<ModelComponent>();
	modelComponent_->SetModel(model_);
	modelComponent_->SetTransformComponent(transformComponent_);

	//コライダーの追加
	OBBCollider* collider = AddComponent<OBBCollider>();
	collider->SetTransformComponent(transformComponent_);
}

void Enemy::Update()
{
	GameObject::Update();

	ImGui::Begin("Enemy");
	ImGui::DragFloat3("Translation", &transformComponent_->translation_.x, 0.1f);
	ImGui::DragFloat3("Rotation", &transformComponent_->rotation_.x, 0.01f);
	ImGui::DragFloat3("Scale", &transformComponent_->scale_.x, 0.01f);
	ImGui::End();
}

void Enemy::Draw(const Camera& camera)
{
	GameObject::Draw(camera);
}

void Enemy::OnCollision(GameObject* collider)
{
	ImGui::Begin("Enemy");
	ImGui::Text("OnCollision");
	ImGui::End();
}

void Enemy::OnCollisionEnter(GameObject* collider)
{
	ImGui::Begin("Enemy");
	ImGui::Text("OnCollisionEnter");
	ImGui::End();
}

void Enemy::OnCollisionExit(GameObject* collider)
{
	ImGui::Begin("Enemy");
	ImGui::Text("OnCollisionExit");
	ImGui::End();
}