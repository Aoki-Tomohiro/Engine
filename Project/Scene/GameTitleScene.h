#pragma once
#include "Engine/Framework/IScene.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Components/Input.h"
#include "Engine/2D/Sprite.h"
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

	Input* input_ = nullptr;

	std::unique_ptr<Sprite> sprite_ = nullptr;

	Vector4 spriteColor_{ 0.0f,0.0f,0.0f,1.0f };

	bool isTransition_ = false;
	bool isTransitionEnd_ = false;

	float transitionTimer_ = 0.0f;
};

