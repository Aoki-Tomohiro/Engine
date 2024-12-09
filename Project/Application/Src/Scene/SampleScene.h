#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/2D/Sprite.h"
#include "Engine/3D/Camera/CameraManager.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/LevelManager/LevelManager.h"

class SampleScene : public IScene
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

	Camera* camera_ = nullptr;

	GameObjectManager* gameObjectManager_ = nullptr;

	//スプライト
	std::unique_ptr<Sprite> sprite_ = nullptr;
};

