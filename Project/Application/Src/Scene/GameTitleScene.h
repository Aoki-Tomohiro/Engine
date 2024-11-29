#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/2D/Sprite.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/LevelManager/LevelManager.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"
#include "Application/Src/Object/Transition/Transition.h"
#include <numbers>

class GameTitleScene : public IScene
{
public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

private:
	void UpdateCamera();

	void TriggerFadeInAndChangeScene();

private:
	//レンダラー
	Renderer* renderer_ = nullptr;

	//インプット
	Input* input_ = nullptr;

	//オーディオ
	Audio* audio_ = nullptr;

	//カメラ
	Camera* camera_ = nullptr;

	//ゲームオブジェクトマネージャー
	GameObjectManager* gameObjectManager_ = nullptr;

	//パーティクルマネージャー
	ParticleManager* particleManager_ = nullptr;

	//敵
	Enemy* enemy_ = nullptr;

	//トランジション
	std::unique_ptr<Transition> transition_ = nullptr;

	//タイトルのスプライト
	std::unique_ptr<Sprite> titleSprite_ = nullptr;
	std::unique_ptr<Sprite> pressASprite_ = nullptr;

	//オーディオハンドル
	uint32_t audioHandle_ = 0;
};

