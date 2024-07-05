#include "Warning.h"

void Warning::Initialize()
{
	//GameObjectの初期化
	GameObject::Initialize();
}

void Warning::Update()
{
	//Gameobjectの更新
	GameObject::Update();
}

void Warning::Draw(const Camera& camera)
{
	//GameObjectの描画
	GameObject::Draw(camera);
}

void Warning::DrawUI()
{

}

void Warning::OnCollision(GameObject* gameObject)
{
}

void Warning::OnCollisionEnter(GameObject* gameObject)
{
}

void Warning::OnCollisionExit(GameObject* gameObject)
{
}