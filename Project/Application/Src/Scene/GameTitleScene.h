#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/2D/Sprite.h"
#include "Engine/Components/PostEffects/PostEffects.h"

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

	//ポストエフェクト
	PostEffects* postEffects_ = nullptr;

	//スプライト
	std::vector<std::unique_ptr<Sprite>> sprites_{};

	//オーディオ
	uint32_t audioHandle_ = 0u;

	//Bloom
	bool isBloomEnable_ = false;
	int blurCount_ = 1;
	ConstBuffDataBloom bloomData_{ false,1.0f,1.0f,1.0f,{1.0f,1.0f,1.0f,1.0f} };

	//Fog
	bool isFogEnable_ = false;
	ConstBuffDataFog fogData_{ false,0.5f,2.0f };

	//DoF
	bool isDoFEnable_ = false;
	ConstBuffDataDoF dofData_{ false,0.02f,0.005f,0.01f };

	//LensDistortion
	bool isLensDistortionEnable_ = false;
	ConstBuffDataLensDistortion lensDistortionData_{ false,2.5f,-0.1f };

	//Vignette
	bool isVignetteEnable_ = false;
	ConstBuffDataVignette vignetteData_{ false,1.0f };
};

