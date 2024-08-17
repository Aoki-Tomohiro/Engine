#include "ModelComponent.h"
#include "Engine/3D/Model/ModelManager.h"

void ModelComponent::Initialize()
{
	model_ = ModelManager::CreateFromModelFile("Cube", Opaque);
}

void ModelComponent::Update()
{
	assert(transformComponent_);
	transformComponent_->Update();
	model_->Update(transformComponent_->worldTransform_, animationName_);
}

void ModelComponent::Draw(const Camera& camera)
{
	assert(transformComponent_);
	model_->Draw(transformComponent_->worldTransform_, camera);
}