#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/2D/Sprite.h"
#include "Engine/3D/Camera/DebugCamera.h"

class GameTitleScene : public IScene
{
public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

private:
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	LightManager* lightManager_ = nullptr;

	//カメラ
	Camera camera_{};

	//デバッグカメラ
	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;

	//地形
	std::unique_ptr<Model> terrainModel_ = nullptr;
	WorldTransform terrainWorldTransform_{};

	//モンスターボール
	std::unique_ptr<Model> monsterBallModel_ = nullptr;
	WorldTransform monsterBallWorldTransform_{};

	//Material
	bool enableLighting_ = true;
	Model::DiffuseReflectionType diffuseReflectionType_ = Model::DiffuseReflectionType::HalfLambert;
	Model::SpecularReflectionType specularReflectionType_ = Model::SpecularReflectionType::BlinnPhongReflectionModel;
	float shininess_ = 40.0f;
	Vector3 specularColor_ = { 1.0f,1.0f,1.0f };

	//DirectionaLight
	bool isDirectionalLightEnable_[2] = { true,false };
	ConstBuffDataDirectionalLight directionalLightData_[2] = {
		{ { 1.0f,1.0f,1.0f,1.0f } ,{ 0.0f,-1.0f,0.0f } ,1.0f ,false },
		{ { 1.0f,1.0f,1.0f,1.0f } ,{ 0.0f,-1.0f,0.0f } ,1.0f ,false }
	};

	//PointLight
	bool isPointLightEnable_[2] = { false,false };
	ConstBuffDataPointLight pointLightData_[2] = {
		{ { 1.0f,1.0f,1.0f,1.0f } ,{ 0.0f,2.0f,0.0f } ,1.0f ,10.0f ,1.0f ,false},
		{ { 1.0f,1.0f,1.0f,1.0f } ,{ 0.0f,2.0f,0.0f } ,1.0f ,10.0f ,1.0f ,false}
	};

	//SpotLight
	bool isSpotLightEnable_[2] = { false,false };
	ConstBuffDataSpotLight spotLightData_[2] = {
		{{ 1.0f,1.0f,1.0f,1.0f } ,{ 0.0f,2.0f,0.0f } ,1.0f ,{ 0.0f,-1.0f,0.0f } ,10.0f ,1.0f ,1.0f ,0.5f ,false},
		{{ 1.0f,1.0f,1.0f,1.0f } ,{ 0.0f,2.0f,0.0f } ,1.0f ,{ 0.0f,-1.0f,0.0f } ,10.0f ,1.0f ,1.0f ,0.5f ,false}
	};
};

