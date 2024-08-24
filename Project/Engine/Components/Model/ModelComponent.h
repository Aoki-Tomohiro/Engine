#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/Transform/WorldTransform.h"
#include "Engine/Components/Base/RenderComponent.h"

class ModelComponent : public RenderComponent
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	Model* GetModel() const { return model_.get(); };

	void SetModel(Model* model) { model_.reset(model); };

private:
	std::unique_ptr<Model> model_ = nullptr;
};

