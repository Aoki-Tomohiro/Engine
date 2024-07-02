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
	//SpecularColorを設定
	groundModelComponent->GetModel()->GetMaterial(0)->SetSpecularColor(specularColor_);

	//GameObjectの更新
	GameObject::Update();

	//ImGui
	ImGui::Begin("Ground");
	ImGui::ColorEdit3("SpecularColor", &specularColor_.x);
	ImGui::End();
}

void Ground::Draw(const Camera& camera)
{
	//GameObjectの描画
	GameObject::Draw(camera);
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