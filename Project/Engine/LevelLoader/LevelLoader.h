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
			struct ColliderData
			{
				std::string type;
				Vector3 center;
				Vector3 size;
			};
			std::string objectName;
			std::string modelName;
			Vector3 translation;
			Vector3 rotation;
			Vector3 scaling;
			ColliderData colliderData;
		};
		struct CameraData
		{
			std::string name;
			Vector3 translation;
			Vector3 rotation;
		};
		std::vector<ObjectData> objects;
		std::vector<CameraData> cameras;
	};

	static LevelLoader* GetInstance();

	static void Load(const std::string& fileName);

private:
	LevelLoader() = default;
	~LevelLoader() = default;
	LevelLoader(const LevelLoader&) = delete;
	const LevelLoader& operator=(const LevelLoader&) = delete;

	void LoadInternal(const std::string& fileName);

	void ProcessObject(const nlohmann::json& object, LevelData* levelData);

	void CreateGameObjects(const LevelData* levelData);

private:
	static const std::string kBaseDirectory;

	std::map<std::string, std::unique_ptr<LevelData>> levelDatas_{};
};

