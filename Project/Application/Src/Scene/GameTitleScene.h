#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/3D/Camera/CameraManager.h"
#include "Engine/3D/Skybox/Skybox.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/LevelLoader/LevelLoader.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Transition/Transition.h"

class GameTitleScene : public IScene
{
public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

private:
	void CameraUpdate();

	void HandleTransition();

private:
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	//Camera
	Camera* camera_ = nullptr;

	//GameObjectManager
	GameObjectManager* gameObjectManager_ = nullptr;

	//ParticleManager
	ParticleManager* particleManager_ = nullptr;

	//トランジション
	std::unique_ptr<Transition> transition_ = nullptr;

	//Skybox
	std::unique_ptr<Skybox> skybox_ = nullptr;

	//タイトルのスプライト
	std::unique_ptr<Sprite> titleSprite_ = nullptr;
	std::unique_ptr<Sprite> pressASprite_ = nullptr;

	//オーディオハンドル
	uint32_t audioHandle_ = 0;
};

