#include "Ground.h"
#include "Engine/Components/Component/ModelComponent.h"

void Ground::Initialize()
{
	//GameObjectの初期化
	GameObject::Initialize();
}

void Ground::Update()
{
	//Modelを取得
	ModelComponent* groundModelComponent = GetComponent<ModelComponent>();
	//色を設定
	groundModelComponent->GetModel()->GetMaterial(0)->SetColor(color_);
	//SpecularColorを設定
	groundModelComponent->GetModel()->GetMaterial(0)->SetSpecularColor(specularColor_);
	//UVScaleを設定
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	groundModelComponent->GetModel()->GetMaterial(0)->SetUVScale({ transformComponent->worldTransform_.scale_.x,transformComponent->worldTransform_.scale_.z });

	//GameObjectの更新
	GameObject::Update();

	//ImGui
	ImGui::Begin("Ground");
	ImGui::ColorEdit3("Color", &color_.x);
	ImGui::ColorEdit3("SpecularColor", &specularColor_.x);
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