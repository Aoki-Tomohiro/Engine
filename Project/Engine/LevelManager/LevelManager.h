#pragma once
#include "Engine/Math/MathFunction.h"
#include <iostream>
#include <fstream>
#include <variant>
#include <string>
#include <map>
#include <Engine/Externals/nlohmann/json.hpp>

class LevelManager
{
public:
    struct ColliderData
    {
        std::string type{};
        std::string attribute{};
        Vector3 center{};
        Vector3 size{};
        float radius = 1.0f;
    };

    struct ObjectData
    {
        std::string objectName{};
        std::string modelName{};
        Vector3 translation{};
        Vector3 rotation{};
        Vector3 scaling{};
        bool isVisible = true;
        ColliderData colliderData{};
    };

    struct CameraData
    {
        std::string name{};
        Vector3 translation{};
        Vector3 rotation{};
    };

    struct LevelData 
    {
        std::vector<ObjectData> objects{};
        std::vector<CameraData> cameras{};
    };

	static LevelManager* GetInstance();

    static void LoadLevelAndCreateObjects(const std::string& fileName);

private:
	LevelManager() = default;
	~LevelManager() = default;
	LevelManager(const LevelManager&) = delete;
	const LevelManager& operator=(const LevelManager&) = delete;

	void LoadLevelAndCreateGameObjectsInternal(const std::string& fileName);

	void ProcessObject(const nlohmann::json& object, LevelData* levelData);

	void CreateGameObjects(const LevelData* levelData);

private:
	static const std::string kBaseDirectory;

	std::map<std::string, std::unique_ptr<LevelData>> levelDatas_{};
};

