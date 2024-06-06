#include "Collider.h"
#include "Engine/Framework/Object/IGameObject.h"

void Collider::OnCollision(Collider* collider)
{
	gameObject_->OnCollision(collider);
}

const Vector3& Collider::GetWorldPosition() const
{
	return { gameObject_->GetWorldTransform().matWorld_.m[3][0],gameObject_->GetWorldTransform().matWorld_.m[3][01] ,gameObject_->GetWorldTransform().matWorld_.m[3][2] };
}

const WorldTransform& Collider::GetWorldTransform() const
{
	return gameObject_->GetWorldTransform();
}