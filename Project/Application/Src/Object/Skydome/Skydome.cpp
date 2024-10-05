#include "Skydome.h"
#include "Engine/Components/Model/ModelComponent.h"

void Skydome::Initialize()
{
	//モデルを取得
	ModelComponent* modelComponent = GetComponent<ModelComponent>();
	//マテリアルの更新
	modelComponent->GetModel()->SetCastShadows(false);
	for (int32_t i = 0; i < modelComponent->GetModel()->GetNumMaterials(); ++i)
	{
		modelComponent->GetModel()->GetMaterial(i)->SetEnableLighting(false);
		modelComponent->GetModel()->GetMaterial(i)->SetEnvironmentCoefficient(0.0f);
		modelComponent->GetModel()->GetMaterial(i)->SetUVScale({ 10.0f,10.0f });
	}
}

void Skydome::OnCollision(GameObject* gameObject)
{
}

void Skydome::OnCollisionEnter(GameObject* gameObject)
{
}

void Skydome::OnCollisionExit(GameObject* gameObject)
{
}