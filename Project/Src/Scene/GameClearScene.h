#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/2D/Sprite.h"
#include "Engine/Math/MathFunction.h"

class GameClearScene : public IScene 
{
public:
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

	//トランジション関連
	std::unique_ptr<Sprite> transitionSprite_ = nullptr;
	Vector4 transitionSpriteColor_{ 0.0f,0.0f,0.0f,1.0f };
	float transitionTimer_ = 0;
	bool isFadeIn_ = false;
	bool isFadeOut_ = true;

	//ゲームクリアのスプライト
	std::unique_ptr<Sprite> gameClearSprite_ = nullptr;
};

