#include "GameClearScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Math/MathFunction.h"

void GameClearScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	gameObjectManager_ = GameObjectManager::GetInstance();
	gameObjectManager_->Initialize();

	//スプライトの生成
	sprite_.reset(Sprite::Create("Project/Resources/Images/white.png", { 0.0f,0.0f }));
	backGroundSprite_.reset(Sprite::Create("Project/Resources/Images/GameClear.png", { 0.0f,0.0f }));
	sprite_->SetSize({ 1280.0f,720.0f });
	sprite_->SetColor(spriteColor_);

	camera_.Initialize();

	skydomeModel_ = ModelManager::CreateFromOBJ("Skydome", Opaque);
	skydome_ = GameObjectManager::CreateGameObject<Skydome>();
	skydome_->SetModel(skydomeModel_);
}

void GameClearScene::Finalize()
{

}

void GameClearScene::Update()
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

	//ゲームオブジェクトの更新
	gameObjectManager_->Update();

	ImGui::Begin("GameClearScene");
	ImGui::Text("A : GameTitleScene");
	ImGui::End();
}

void GameClearScene::Draw()
{
	renderer_->PreDrawModels();

	gameObjectManager_->Draw(camera_);

	renderer_->PostDrawModels();
}

void GameClearScene::DrawUI()
{
	renderer_->PreDrawSprites(Renderer::kBlendModeNormal);

	backGroundSprite_->Draw();

	sprite_->Draw();

	renderer_->PostDrawSprites();
}