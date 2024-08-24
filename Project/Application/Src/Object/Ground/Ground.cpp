#include "Ground.h"
#include "Engine/Components/Model/ModelComponent.h"

void Ground::Initialize()
{
	//GameObjectの初期化
	GameObject::Initialize();
}

void Ground::Update()
{
	//モデルを取得
	ModelComponent* modelComponent = GetComponent<ModelComponent>();
	//マテリアルの更新
	modelComponent->GetModel()->GetMaterial(0)->SetColor(color_);
	modelComponent->GetModel()->GetMaterial(0)->SetSpecularColor(specularColor_);
	modelComponent->GetModel()->GetMaterial(0)->SetUVScale(uvScale_);

	//GameObjectの更新
	GameObject::Update();

	//ImGui
	ImGui::Begin("Ground");
	ImGui::ColorEdit3("Color", &color_.x);
	ImGui::ColorEdit3("SpecularColor", &specularColor_.x);
	ImGui::DragFloat2("UVScale", &uvScale_.x);
	ImGui::End();
}

void Ground::Draw(const Camera& camera)
{
	//GameObjectの描画
	GameObject::Draw(camera);
}

void Ground::DrawUI()
{

}

void Ground::OnCollision(GameObject* gameObject)
{

}

void Ground::OnCollisionEnter(GameObject* gameObject)
{

}

void Ground::OnCollisionExit(GameObject* gameObject)
{

}