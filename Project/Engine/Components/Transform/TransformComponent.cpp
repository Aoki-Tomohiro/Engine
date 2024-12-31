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