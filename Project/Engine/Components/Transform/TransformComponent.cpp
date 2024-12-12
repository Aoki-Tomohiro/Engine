/**
 * @file TransformComponent.cpp
 * @brief トランスフォームのコンポーネント
 * @author 青木智滉
 * @date
 */

#include "TransformComponent.h"
#include "Engine/Math/MathFunction.h"

void TransformComponent::Initialize()
{
	worldTransform_.Initialize();
	worldTransform_.UpdateMatrix();
}

void TransformComponent::Update()
{
	worldTransform_.UpdateMatrix();
}

const Vector3 TransformComponent::GetWorldPosition() const
{
	Vector3 worldPosition{};
	worldPosition.x = worldTransform_.matWorld_.m[3][0];
	worldPosition.y = worldTransform_.matWorld_.m[3][1];
	worldPosition.z = worldTransform_.matWorld_.m[3][2];
	return worldPosition;
}
