#include "Enemy.h"

void Enemy::Initialize()
{
	//GameObjectの初期化
	GameObject::Initialize();
}

void Enemy::Update()
{
	//GameObjectの更新
	GameObject::Update();
}

void Enemy::Draw(const Camera& camera)
{
	//GameObjectの描画
	GameObject::Draw(camera);
}

void Enemy::OnCollision(GameObject* gameObject)
{

}

void Enemy::OnCollisionEnter(GameObject* gameObject)
{

}

void Enemy::OnCollisionExit(GameObject* gameObject)
{

}