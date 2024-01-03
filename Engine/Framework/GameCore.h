#pragma once
#include "Engine/Base/Application.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/3D/ModelManager.h"
#include "Engine/Components/Input.h"
#include "Engine/Components/Audio.h"
#include "Engine/Components/ParticleManager.h"
#include "Engine/Components/PostProcess.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Framework/GameObjectManager.h"
#include "Engine/Utilities/D3DResourceLeakChecker.h"

class GameCore
{
public:
	virtual ~GameCore() = default;

	virtual void Initialize();

	virtual void Finalize();

	virtual void Update();

	virtual void Draw();

	virtual bool IsEndRequest();

	void Run();

protected:
	D3DResourceLeakChecker leakChecker{};

	Application* application_ = nullptr;

	GraphicsCore* graphicsCore_ = nullptr;

	TextureManager* textureManager_ = nullptr;

	ImGuiManager* imguiManager_ = nullptr;

	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	PostProcess* postProcess_ = nullptr;

	SceneManager* sceneManager_ = nullptr;

	std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;
};

