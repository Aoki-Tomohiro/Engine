#include "MagicProjectile.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Utilities/GameTimer.h"

void MagicProjectile::Initialize()
{
	//基底クラスの初期化
	GameObject::Initialize();
}

void MagicProjectile::Update()
{
	//移動処理
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.translation_ += velocity_ * GameTimer::GetDeltaTime();

	//基底クラスの更新
	GameObject::Update();
}

void MagicProjectile::Draw(const Camera& camera)
{
	//基底クラスの描画
	GameObject::Draw(camera);
}

void MagicProjectile::DrawUI()
{
}

void MagicProjectile::OnCollision(GameObject* gameObject)
{
	//破壊フラグを立てる
	SetIsDestroy(true);
}

void MagicProjectile::OnCollisionEnter(GameObject* gameObject)
{
}

void MagicProjectile::OnCollisionExit(GameObject* gameObject)
{
}