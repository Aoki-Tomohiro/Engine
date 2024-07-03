#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/3D/Skybox/Skybox.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Component/ModelComponent.h"
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

	//GameObjectManager
	GameObjectManager* gameObjectManager_ = nullptr;

	//カメラ
	Camera camera_{};

	//トランジション
	std::unique_ptr<Transition> transition_ = nullptr;

	//Skybox
	std::unique_ptr<Skybox> skybox_ = nullptr;
};

