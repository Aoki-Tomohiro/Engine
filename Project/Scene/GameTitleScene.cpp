#include "GameTitleScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Components/Audio.h"

void GameTitleScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	gameObjectManager_ = GameObjectManager::GetInstance();
	gameObjectManager_->Initialize();

	//スプライトの生成
	sprite_.reset(Sprite::Create("Project/Resources/Images/white.png", { 0.0f,0.0f }));
	backGroundSprite_.reset(Sprite::Create("Project/Resources/Images/Title.png", { 0.0f,0.0f }));
	sprite_->SetSize({ 1280.0f,720.0f });
	sprite_->SetColor(spriteColor_);

	camera_.Initialize();

	skydomeModel_ = ModelManager::CreateFromOBJ("Skydome", Opaque);
	skydome_ = GameObjectManager::CreateGameObject<Skydome>();
	skydome_->SetModel(skydomeModel_);

	audioHandle_ = Audio::GetInstance()->SoundLoadWave("Project/Resources/Sounds/Title.wav");
	Audio::GetInstance()->SoundPlayWave(audioHandle_, true, 0.5f);
}

void GameTitleScene::Finalize()
{

}

void GameTitleScene::Update()
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
			sceneManager_->ChangeScene("GamePlayScene");
			Audio::GetInstance()->StopAudio(audioHandle_);
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

		if (input_->IsPushKeyEnter(DIK_SPACE))
		{
			isTransition_ = true;
		}
	}

	//ゲームオブジェクトの更新
	gameObjectManager_->Update();

	ImGui::Begin("GameTitleScene");
	ImGui::Text("A : GamePlayScene");
	ImGui::End();
}

void GameTitleScene::Draw()
{
	renderer_->PreDrawModels();

	gameObjectManager_->Draw(camera_);

	renderer_->PostDrawModels();
}

void GameTitleScene::DrawUI()
{
	renderer_->PreDrawSprites(Renderer::kBlendModeNormal);

	backGroundSprite_->Draw();

	sprite_->Draw();

	renderer_->PostDrawSprites();
}