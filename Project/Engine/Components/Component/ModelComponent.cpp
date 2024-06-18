#include "ModelComponent.h"

void ModelComponent::Initialize()
{
	worldTransform_.Initialize();
}

void ModelComponent::Update()
{
	if (transformComponent_ != nullptr)
	{
		worldTransform_.translation_ = transformComponent_->translation_;
		worldTransform_.rotation_ = transformComponent_->rotation_;
		worldTransform_.quaternion_ = transformComponent_->quaternion_;
		worldTransform_.scale_ = transformComponent_->scale_;
		worldTransform_.rotationType_ = transformComponent_->rotationType_;
	}

	worldTransform_.UpdateMatrix();

	if (model_ != nullptr)
	{
		model_->Update(worldTransform_, animationName_);
	}
}

void ModelComponent::Draw(const Camera& camera)
{
	if (model_ != nullptr)
	{
		model_->Draw(worldTransform_, camera);
	}
}