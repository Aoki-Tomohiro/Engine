#include "Collider.h"
#include "Engine/Framework/Object/IGameObject.h"

void Collider::OnCollision(Collider* collider)
{
	gameObject_->OnCollision(collider);
}

const Vector3 Collider::GetWorldPosition() const
{
	Vector3 worldPosition{};
	worldPosition.x = gameObject_->GetWorldTransform().matWorld_.m[3][0];
	worldPosition.y = gameObject_->GetWorldTransform().matWorld_.m[3][1];
	worldPosition.z = gameObject_->GetWorldTransform().matWorld_.m[3][2];
	return worldPosition;
}

const WorldTransform& Collider::GetWorldTransform() const
{
	return gameObject_->GetWorldTransform();
}