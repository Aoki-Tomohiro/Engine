#pragma once
#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>

class ModelManager
{
public:
	static const std::string kBaseDirectory;

	static ModelManager* GetInstance();

	static void Destroy();

	static Model* CreateFromModelFile(const std::string& modelName, DrawPass drawPass);

private:
	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager&) = delete;

	Model* CreateInternal(const std::string& modelName, DrawPass drawPass);

	Model* FindReusableModel(const std::string& modelName);

	Model* CreateModelFromData(const Model::ModelData& modelData, const std::string& modelName, DrawPass drawPass);

	std::string FindModelFile(const std::string& modelName);

	Model::ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);

	Model::Node ReadNode(aiNode* node);

private:
	static ModelManager* instance_;

	std::map<std::string, Model::ModelData> modelDatas_{};

	std::map<std::string, std::vector<std::unique_ptr<Model>>> models_{};
};

