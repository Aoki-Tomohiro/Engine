#include "Transition.h"

void Transition::Initialize()
{
	//スプライトの生成
	sprite_.reset(Sprite::Create("white.png", { 0.0f,0.0f }));

	//スプライトのサイズを設定
	sprite_->SetSize({ 1280.0f,720.0f });
}

void Transition::Update()
{
	//フェード状態に応じてアルファ値を更新
	UpdateAlphaValue();

	//アルファ値に基づいてフラグを設定
	UpdateFadeFlags();

	//スプライトの色の更新
	sprite_->SetColor(spriteColor_);
}

void Transition::Draw()
{
	//スプライトの描画
	sprite_->Draw();
}

void Transition::UpdateAlphaValue()
{
	//現在のフェードの状態に応じてアルファ値を変更
	switch (fadeState_)
	{
	case FadeState::In:
		spriteColor_.w = std::min<float>(spriteColor_.w + transitionSpeed_, kMaxAlpha);
		break;
	case FadeState::Out:
		spriteColor_.w = std::max<float>(spriteColor_.w - transitionSpeed_, kMinAlpha);
		break;
	}
}

void Transition::UpdateFadeFlags()
{
	//アルファ値に基づいてフラグを設定
	isFadeOutComplete_ = (spriteColor_.w <= kMinAlpha);
	isFadeInComplete_ = (spriteColor_.w >= kMaxAlpha);
}