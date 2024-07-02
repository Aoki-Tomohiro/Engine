#include "TransformComponent.h"
#include "Engine/Math/MathFunction.h"

void TransformComponent::Initialize()
{

}

void TransformComponent::Update()
{
	switch (worldTransform_.rotationType_)
	{
	case RotationType::Euler:
		worldTransform_.matWorld_ = Mathf::MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
		break;
	case RotationType::Quaternion:
		worldTransform_.matWorld_ = Mathf::MakeAffineMatrix(worldTransform_.scale_, worldTransform_.quaternion_, worldTransform_.translation_);
		break;
	}

	if (worldTransform_.parent_)
	{
		worldTransform_.matWorld_ = worldTransform_.matWorld_ * worldTransform_.parent_->matWorld_;
	}
}

const Vector3 TransformComponent::GetWorldPosition()
{
	Vector3 worldPosition{};
	worldPosition.x = worldTransform_.matWorld_.m[3][0];
	worldPosition.y = worldTransform_.matWorld_.m[3][1];
	worldPosition.z = worldTransform_.matWorld_.m[3][2];
	return worldPosition;
}
