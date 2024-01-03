#include "GameOverScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"

void GameOverScene::Initialize()
{
	input_ = Input::GetInstance();
}

void GameOverScene::Finalize()
{

}

void GameOverScene::Update()
{
	if (input_->IsControllerConnected())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			sceneManager_->ChangeScene("GamePlayScene");
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

}