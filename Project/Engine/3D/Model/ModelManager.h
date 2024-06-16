#pragma once
#include "Model.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

	Model::ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);

	std::vector<Animation::AnimationData> LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

	Animation::Node ReadNode(aiNode* node);

private:
	static ModelManager* instance_;

	std::map<std::string, std::pair<Model::ModelData, std::vector<Animation::AnimationData>>> models_{};
};

