#pragma once
#include "Engine/2D/Sprite.h"

class Transition
{
public:
	enum FadeState { In, Out };

	void Initialize();

	void Update();

	void Draw();

	void SetTransitionSpeed(const float transitionSpeed) { transitionSpeed_ = transitionSpeed; };

	const FadeState& GetFadeState() const { return fadeState_; };

	void SetFadeState(const FadeState& fadeState) { fadeState_ = fadeState; };

	const bool GetIsFadeInComplete() const { return isFadeInComplete_; };

	const bool GetIsFadeOutComplete() const { return isFadeOutComplete_; };

private:
	//アルファ値の最低値
	const float kMinAlpha = 0.0f;

	//アルファ価の最大値
	const float kMaxAlpha = 1.0f;

	void UpdateAlphaValue();

	void UpdateFadeFlags();

private:
	//トランジションのスプライト
	std::unique_ptr<Sprite> sprite_ = nullptr;

	//トランジションのスプライトの色
	Vector4 spriteColor_ = { 0.0f,0.0f,0.0f,1.0f };

	//トランジションの速度
	float transitionSpeed_ = 0.1f;

	//フェードの状態
	FadeState fadeState_ = FadeState::Out;

	//フェードインが終了しているかどうか
	bool isFadeInComplete_ = false;

	//フェードアウトが終了しているかどうか
	bool isFadeOutComplete_ = false;
};

