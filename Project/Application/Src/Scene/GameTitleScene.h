#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
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
	Camera* camera_ = nullptr;

	//トランジション
	std::unique_ptr<Transition> transition_ = nullptr;
};

