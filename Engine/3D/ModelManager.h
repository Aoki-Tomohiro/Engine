#pragma once
#include "Model.h"
#include <filesystem>
#include <unordered_map>

class ModelManager
{
public:
	//ベースディレクトリ
	static const std::string kBaseDirectory;

	static ModelManager* GetInstance();

	static void Destroy();

	static Model* CreateFromOBJ(const std::string& modelName, DrawPass drawPass);

private:
	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager&) = delete;

private:
	Model* CreateInternal(const std::string& modelName, DrawPass drawPass);

private:
	static ModelManager* instance_;

	std::unordered_map<std::string, std::unique_ptr<Model>> models_;
};

