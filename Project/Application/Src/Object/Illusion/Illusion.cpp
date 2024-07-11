#include "Illusion.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"

void Illusion::Initialize()
{
	//GameObjectの初期化
	GameObject::Initialize();
}

void Illusion::Update()
{
	//徐々に薄くする
	color_.w -= GameTimer::GetDeltaTime();

	//モデルの色を設定
	ModelComponent* modelComponent = GetComponent<ModelComponent>();
	modelComponent->GetModel()->GetMaterial(0)->SetColor(color_);

	//完全に透明になったらオブジェクト削除
	if (color_.w < 0.0f)
	{
		SetIsDestroy(true);
	}

	//GameObjectの更新
	GameObject::Update();
}

void Illusion::Draw(const Camera& camera)
{
	//GameObjectの描画
	GameObject::Draw(camera);
}

void Illusion::DrawUI()
{
	//GameObjectnoUI描画
	GameObject::DrawUI();
}

void Illusion::OnCollision(GameObject* gameObject)
{
}

void Illusion::OnCollisionEnter(GameObject* gameObject)
{
}

void Illusion::OnCollisionExit(GameObject* gameObject)
{
}
