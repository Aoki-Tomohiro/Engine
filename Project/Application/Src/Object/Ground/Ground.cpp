#include "Ground.h"

void Ground::Initialize()
{
	GameObject::Initialize();
}

void Ground::Update()
{
	GameObject::Update();
}

void Ground::Draw(const Camera& camera)
{
	GameObject::Draw(camera);
}

void Ground::OnCollision(GameObject* collider)
{

}

void Ground::OnCollisionEnter(GameObject* collider)
{

}

void Ground::OnCollisionExit(GameObject* collider)
{

}