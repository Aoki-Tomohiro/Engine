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