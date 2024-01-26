#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/2D/Sprite.h"
#include "Engine/Math/MathFunction.h"

#include "Project/Src/Object/Skydome/Skydome.h"
#include "Project/Src/Object/Ground/Ground.h"
#include "Project/Src/Object/Player/Player.h"
#include "Project/Src/Object/Boss/Boss.h"
#include "Project/Src/Object/FollowCamera/FollowCamera.h"
#include "Project/Src/Object/LockOn/LockOn.h"

class GamePlayScene : public IScene
{
public:
	enum NextScene
	{
		kGameClear,
		kGameOver,
	};

	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

	void UpdateTransition();

private:
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	GameObjectManager* gameObjectManager_ = nullptr;

	Camera camera_{};

	//ロックオン
	std::unique_ptr<LockOn> lockOn_ = nullptr;

	//追従カメラ
	std::unique_ptr<FollowCamera> followCamera_ = nullptr;

	//プレイヤー
	std::unique_ptr<Model> playerModel_ = nullptr;
	Player* player_ = nullptr;

	//ボス
	std::unique_ptr<Model> bossModel_ = nullptr;
	Boss* boss_ = nullptr;

	//天球
	std::unique_ptr<Model> skydomeModel_ = nullptr;
	Skydome* skydome_ = nullptr;

	//地面
	std::unique_ptr<Model> groundModel_ = nullptr;
	Ground* ground_ = nullptr;

	//トランジション関連
	std::unique_ptr<Sprite> transitionSprite_ = nullptr;
	Vector4 transitionSpriteColor_{ 0.0f,0.0f,0.0f,1.0f };
	float transitionTimer_ = 0;
	bool isFadeIn_ = false;
	bool isFadeOut_ = true;

	//BGM
	uint32_t bgmHandle_ = 0;

	//次のシーン
	NextScene nextScene_ = kGameClear;
};

