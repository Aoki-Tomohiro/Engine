#include "GameOverScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Math/MathFunction.h"

void GameOverScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	//スプライトの生成
	sprite_.reset(Sprite::Create("Project/Resources/Images/white.png", { 0.0f,0.0f }));
	sprite_->SetSize({ 1280.0f,720.0f });
	sprite_->SetColor(spriteColor_);
}

void GameOverScene::Finalize()
{

}

void GameOverScene::Update()
{
	if (!isTransitionEnd_)
	{
		transitionTimer_ += 1.0f / kTransitionTime;
		spriteColor_.w = Mathf::Lerp(spriteColor_.w, 0.0f, transitionTimer_);
		sprite_->SetColor(spriteColor_);

		if (spriteColor_.w <= 0.0f)
		{
			isTransitionEnd_ = true;
			transitionTimer_ = 0.0f;
		}

	}

	if (isTransition_)
	{
		transitionTimer_ += 1.0f / kTransitionTime;
		spriteColor_.w = Mathf::Lerp(spriteColor_.w, 1.0f, transitionTimer_);
		sprite_->SetColor(spriteColor_);

		if (spriteColor_.w >= 1.0f)
		{
			sceneManager_->ChangeScene("GameTitleScene");
		}
	}

	if (isTransitionEnd_)
	{
		if (input_->IsControllerConnected())
		{
			if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
			{
				isTransition_ = true;
			}
		}
	}

	ImGui::Begin("GameOverScene");
	ImGui::Text("A : GameTitleScene");
	ImGui::End();
}

void GameOverScene::Draw()
{

}

void GameOverScene::DrawUI()
{
	renderer_->PreDrawSprites(Renderer::kBlendModeNormal);

	sprite_->Draw();

	renderer_->PostDrawSprites();
}