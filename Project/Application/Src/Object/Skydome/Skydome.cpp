#include "Skydome.h"

void Skydome::Initialize()
{
	//モデルを取得
	model_ = GetComponent<ModelComponent>();

	//基底クラスの初期化
	GameObject::Initialize();
}

void Skydome::Update()
{
	//影を描画するかどうかを設定
	model_->GetModel()->SetCastShadows(castShadows_);

	//マテリアルの更新
	for (int32_t i = 0; i < model_->GetModel()->GetNumMaterials(); ++i)
	{
		model_->GetModel()->GetMaterial(i)->SetEnableLighting(isEnableLighting_);
		model_->GetModel()->GetMaterial(i)->SetEnvironmentCoefficient(environmentCoefficient_);
		model_->GetModel()->GetMaterial(i)->SetUVScale(uvScale_);
	}

	//基底クラスの更新
	GameObject::Update();
}