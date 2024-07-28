#include "BackgroundObject.h"

void BackgroundObject::Initialize()
{
	//基底クラスの初期化
	GameObject::Initialize();
}

void BackgroundObject::Update()
{
	//基底クラスの初期化
	GameObject::Update();
}

void BackgroundObject::Draw(const Camera& camera)
{
	//基底クラスの描画
	GameObject::Draw(camera);
}

void BackgroundObject::DrawUI()
{
}

void BackgroundObject::OnCollision(GameObject* gameObject)
{
}

void BackgroundObject::OnCollisionEnter(GameObject* gameObject)
{
}

void BackgroundObject::OnCollisionExit(GameObject* gameObject)
{
}