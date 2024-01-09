#pragma once
#include "Engine/Framework/IScene.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Components/Input.h"
#include "Engine/2D/Sprite.h"
#include "Engine/Framework/GameObjectManager.h"
#include "Engine/3D/ModelManager.h"
#include "Project/Object/Skydome/Skydome.h"
#include <memory>

class GameTitleScene : public IScene
{
public:
	//トランジションの時間
	static const int kTransitionTime = 60;

	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

private:
	Renderer* renderer_ = nullptr;

	GameObjectManager* gameObjectManager_ = nullptr;

	Input* input_ = nullptr;

	//トランジション
	std::unique_ptr<Sprite> sprite_ = nullptr;
	Vector4 spriteColor_{ 0.0f,0.0f,0.0f,1.0f };
	bool isTransition_ = false;
	bool isTransitionEnd_ = false;
	float transitionTimer_ = 0.0f;

	//背景
	std::unique_ptr<Sprite> backGroundSprite_ = nullptr;

	//天球
	Model* skydomeModel_ = nullptr;
	Skydome* skydome_ = nullptr;

	//カメラ
	Camera camera_{};

	//オーディオハンドル
	uint32_t audioHandle_ = 0;
};

