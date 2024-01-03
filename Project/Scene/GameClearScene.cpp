#include "GameClearScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"

void GameClearScene::Initialize()
{
	input_ = Input::GetInstance();
}

void GameClearScene::Finalize()
{

}

void GameClearScene::Update()
{
	if (input_->IsControllerConnected())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			sceneManager_->ChangeScene("GamePlayScene");
		}
	}

	ImGui::Begin("GameClearScene");
	ImGui::Text("A : GameTitleScene");
	ImGui::End();
}

void GameClearScene::Draw()
{

}

void GameClearScene::DrawUI()
{

}