#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/2D/Sprite.h"

#include "Project/Src/Object/Skydome/Skydome.h"
#include "Project/Src/Object/Ground/Ground.h"
#include "Project/Src/Object/Player/Player.h"
#include "Project/Src/Object/FollowCamera/FollowCamera.h"

class GamePlayScene : public IScene
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

	Camera camera_{};

	//追従カメラ
	std::unique_ptr<FollowCamera> followCamera_ = nullptr;

	//プレイヤー
	std::unique_ptr<Model> playerModel_ = nullptr;
	Player* player_ = nullptr;

	//天球
	std::unique_ptr<Model> skydomeModel_ = nullptr;
	Skydome* skydome_ = nullptr;

	//地面
	std::unique_ptr<Model> groundModel_ = nullptr;
	Ground* ground_ = nullptr;
};

