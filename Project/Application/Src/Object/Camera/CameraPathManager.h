#pragma once
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Utilities/GameTimer.h"
#include "CameraPath.h"
#include <map>
#include <string>
#include <fstream>
#include <Engine/Externals/nlohmann/json.hpp>

class CameraPathManager
{
public:
	void Initialize();

	void Update();

	const bool GetIsEditModeEnabled() const { return isEditModeEnabled_; };

	const bool GetIsPlayAnimation() const { return isPlayAnimation_; };

	const CameraKeyFrame GetCurrentKeyFrame();

	const CameraKeyFrame& GetNewKeyFrame() const { return newKeyFrame_; };

	CameraPath& GetPath(const std::string& name) { return cameraPaths_[name]; };

private:
	const std::string kFileName = "Application/Resources/Config/Camera/CameraPath.json";

	void SaveFiles();

	void LoadFiles();

private:
	std::map<std::string, CameraPath> cameraPaths_{};

	std::string currentPathName = "";

	CameraKeyFrame newKeyFrame_{};

	bool isEditModeEnabled_ = false;

	bool isPlayAnimation_ = false;

	float animationTime_ = 0.0f;
};

