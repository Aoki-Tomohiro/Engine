#include "Enemy.h"

void Enemy::Initialize()
{
	GameObject::Initialize();
}

void Enemy::Update()
{
	GameObject::Update();
}

void Enemy::Draw(const Camera& camera)
{
	GameObject::Draw(camera);
}

void Enemy::OnCollision(GameObject* collider)
{

}

void Enemy::OnCollisionEnter(GameObject* collider)
{

}

void Enemy::OnCollisionExit(GameObject* collider)
{

}