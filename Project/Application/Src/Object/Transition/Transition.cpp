#include "Transition.h"

void Transition::Initialize()
{
	sprite_.reset(Sprite::Create("white.png", { 0.0f,0.0f }));
	sprite_->SetSize({ 1280.0f,720.0f });
}

void Transition::Update()
{
	switch (fadeState_)
	{
	case FadeState::In:
		color_.w += fadeSpeed_;
		break;
	case FadeState::Out:
		color_.w -= fadeSpeed_;
		break;
	}

	if (color_.w <= 0.0f)
	{
		color_.w = 0.0f;
		fadeOutComplete_ = true;
	}
	else if (color_.w >= 1.0f)
	{
		color_.w = 1.0f;
		fadeInComplete_ = true;
	}
	else
	{
		fadeOutComplete_ = false;
		fadeInComplete_ = false;
	}

	sprite_->SetColor(color_);
}

void Transition::Draw()
{
	sprite_->Draw();
}