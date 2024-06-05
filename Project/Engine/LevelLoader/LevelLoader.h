#pragma once
#include <iostream>
#include <fstream>
#include <variant>
#include <string>
#include <map>
#include <Engine/Externals/nlohmann/json.hpp>
#include "Engine/Math/Vector3.h"

class LevelLoader
{
public:
	struct LevelData
	{
		struct ObjectData
		{
			std::string modelName;
			Vector3 translation;
			Vector3 rotation;
			Vector3 scaling;
		};
		std::vector<ObjectData> objects;
	};

	static LevelLoader* GetInstance();

	static void Load(const std::string& fileName);

private:
	LevelLoader() = default;
	~LevelLoader() = default;
	LevelLoader(const LevelLoader&) = delete;
	const LevelLoader& operator=(const LevelLoader&) = delete;

	void LoadInternal(const std::string& fileName);

	void CreateGameObjects(const LevelData* levelData);

private:
	static const std::string kBaseDirectory;

	std::map<std::string, std::unique_ptr<LevelData>> levelDatas_{};
};

