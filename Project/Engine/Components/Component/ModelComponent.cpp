#include "ModelComponent.h"
#include "Engine/3D/Model/ModelManager.h"

void ModelComponent::Initialize(const std::string& modelName, const DrawPass drawPass)
{
	model_ = ModelManager::CreateFromModelFile(modelName, drawPass);

	Initialize();
}

void ModelComponent::Initialize()
{
	worldTransform_.Initialize();
}

void ModelComponent::Update()
{
	if (transformComponent_ != nullptr)
	{
		worldTransform_ = transformComponent_->worldTransform_;
	}

	worldTransform_.UpdateMatrix();

	model_->Update(worldTransform_, animationName_);
}

void ModelComponent::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera);
}