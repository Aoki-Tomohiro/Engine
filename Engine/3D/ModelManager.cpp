#include "ModelManager.h"

//実体定義
ModelManager* ModelManager::instance_ = nullptr;
const std::string ModelManager::kBaseDirectory = "Project/Resources/Models";

ModelManager* ModelManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new ModelManager();
	}
	return instance_;
}

void ModelManager::Destroy()
{
	if (instance_)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

Model* ModelManager::CreateFromOBJ(const std::string& modelName, DrawPass drawPass)
{
	Model* model = ModelManager::GetInstance()->CreateInternal(modelName, drawPass);
	return model;
}

Model* ModelManager::CreateInternal(const std::string& modelName,DrawPass drawPass)
{
	auto it = models_.find(modelName);

	if (it != models_.end())
	{
		return it->second.get();
	}

	Model* model = new Model();
	model->CreateFromOBJ(kBaseDirectory + "/" + modelName, modelName + ".obj", drawPass);
	models_[modelName] = std::unique_ptr<Model>(model);
	return model;
}