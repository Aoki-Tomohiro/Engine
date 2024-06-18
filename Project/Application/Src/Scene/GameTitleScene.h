#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Collision/CollisionManager.h"

#include "Application/Src/Object/Player.h"
#include "Application/Src/Object/Enemy.h"

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

	//カメラ
	Camera camera_{};

	//プレイヤー
	std::unique_ptr<Player> player_ = nullptr;

	//敵
	std::unique_ptr<Enemy> enemy_ = nullptr;

	//衝突マネージャー
	std::unique_ptr<CollisionManager> collisionManager_ = nullptr;
};

