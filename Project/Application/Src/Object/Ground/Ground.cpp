#include "Ground.h"

void Ground::Initialize()
{
	//基底クラスの初期化
	GameObject::Initialize();

	//モデルを取得
	model_ = GetComponent<ModelComponent>();
}

void Ground::Update()
{
	//マテリアルの更新
	model_->GetModel()->GetMaterial(0)->SetColor(color_);
	model_->GetModel()->GetMaterial(0)->SetSpecularColor(specularColor_);
	model_->GetModel()->GetMaterial(0)->SetUVScale(uvScale_);

	//基底クラスの更新
	GameObject::Update();
}