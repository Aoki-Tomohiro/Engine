#pragma once
#include "Animation.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class AnimationManager
{
public:
	static const std::string kBaseDirectory;

	static AnimationManager* GetInstance();

	static void Destroy();

	static Animation* Create(const std::string& fileName);

private:
	AnimationManager() = default;
	~AnimationManager() = default;
	AnimationManager(const AnimationManager&) = delete;
	AnimationManager& operator=(const AnimationManager&) = delete;

	Animation* CreateInternal(const std::string& fileName);

	std::vector<Animation::AnimationData> LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

private:
	static AnimationManager* instance_;

	std::map<std::string, std::vector<Animation::AnimationData>> animationDatas_{};
};

