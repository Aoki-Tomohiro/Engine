#pragma once
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/Transform/WorldTransform.h"

class ModelComponent : public RenderComponent
{
public:
	void Initialize(const std::string& modelName, const DrawPass drawPass);

	void Update() override;

	void Draw(const Camera& camera) override;

	Model* GetModel() const { return model_; };

	void SetModel(Model* model) { model_ = model; };

	void SetAnimationName(const std::string& animationName) { animationName_ = animationName; };

	void SetTransformComponent(TransformComponent* tranformComponent) { transformComponent_ = tranformComponent; };

private:
	void Initialize() override;

private:
	Model* model_ = nullptr;

	WorldTransform worldTransform_{};

	TransformComponent* transformComponent_ = nullptr;

	std::string animationName_ = "";
};

