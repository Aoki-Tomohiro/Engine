#pragma once
#include "Engine/2D/Sprite.h"

class Transition
{
public:
	enum FadeState
	{
		In,
		Out,
	};

	void Initialize();

	void Update();

	void Draw();

	void SetFadeState(const FadeState fadeState) { fadeState_ = fadeState; };

	void SetFadeSpeed(const float speed) { fadeSpeed_ = speed; };

	const bool GetFadeInComplete() const { return fadeInComplete_; };

	const bool GetFadeOutComplete() const { return fadeOutComplete_; };

	const FadeState GetFadeState() const { return fadeState_; };

private:
	//トランジション関連
	std::unique_ptr<Sprite> sprite_ = nullptr;

	//スプライトの色
	Vector4 color_{ 0.0f,0.0f,0.0f,1.0f };

	//フェードの状態
	FadeState fadeState_ = FadeState::Out;

	//タイマー
	float fadeSpeed_ = 0.1f;

	//フラグ
	bool fadeInComplete_ = false;
	bool fadeOutComplete_ = false;
};

