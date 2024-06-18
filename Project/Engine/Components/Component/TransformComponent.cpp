#include "TransformComponent.h"
#include "Engine/Math/MathFunction.h"

void TransformComponent::Initialize()
{
	//Translationの初期化
	translation_ = { 0.0f,0.0f,0.0f };

	//Rotationの初期化
	rotation_ = { 0.0f,0.0f,0.0f };

	//Scaleの初期化
	scale_ = { 1.0f,1.0f,1.0f };

	//Quaternionの初期化
	quaternion_ = Mathf::IdentityQuaternion();
}

void TransformComponent::Update()
{
	
}