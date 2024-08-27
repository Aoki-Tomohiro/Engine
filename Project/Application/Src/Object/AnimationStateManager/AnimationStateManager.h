#pragma once
#include "Engine/Base/ImGuiManager.h"
#include <algorithm>
#include <map>
#include <string>
#include <fstream>
#include <Engine/Externals/nlohmann/json.hpp>

//フェーズ
struct Phase
{
	std::string name{};
	float animationTime = 0.0f;
};

//状態
struct AnimationState
{
	std::string name{};
	std::vector<Phase> phases{};
	char nameBuffer[128]{};
};

class AnimationStateManager
{
public:	
	void Initialize();

	void Update();

	const AnimationState GetAnimationState(const std::string& name) const;

private:
	const std::string kFileName = "Application/Resources/Config/AnimationPhase.json";

	void LoadFiles();

	void SaveFiles();

private:
	//状態とそのフェーズを管理するコンテナ
	std::vector<AnimationState> phaseDatas_;
};

