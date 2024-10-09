#include "ModelComponent.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Framework/Object/GameObject.h"

ModelComponent::~ModelComponent()
{
	if (model_)
	{
		model_->Release();
	}
}

void ModelComponent::Initialize()
{
	model_ = ModelManager::CreateFromModelFile("Cube", Opaque);
}

void ModelComponent::Update()
{
	TransformComponent* transformComponent = owner_->GetComponent<TransformComponent>();
	model_->Update(transformComponent->worldTransform_);
}

void ModelComponent::Draw(const Camera& camera)
{
	TransformComponent* transformComponent = owner_->GetComponent<TransformComponent>();
	model_->Draw(transformComponent->worldTransform_, camera);
}