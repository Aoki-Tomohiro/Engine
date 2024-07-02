#include "Collider.h"
#include "Engine/3D/Primitive/LineRenderer.h"
#include "Engine/Framework/Object/GameObject.h"

void Collider::Initialize()
{

}

void Collider::Update()
{

}

void Collider::Draw(const Camera& camera)
{
	LineRenderer* lineRenderer = LineRenderer::GetInstance();
	lineRenderer->SetCamera(&camera);
}

void Collider::OnCollision(GameObject* other)
{
	owner_->OnCollision(other);
}

void Collider::OnCollisionEnter(GameObject* other)
{
	owner_->OnCollisionEnter(other);
}

void Collider::OnCollisionExit(GameObject* other)
{
	owner_->OnCollisionExit(other);
}