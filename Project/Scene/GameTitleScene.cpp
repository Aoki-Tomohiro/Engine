#include "GameTitleScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"

void GameTitleScene::Initialize()
{
	input_ = Input::GetInstance();
}

void GameTitleScene::Finalize()
{

}

void GameTitleScene::Update()
{
	if (input_->IsControllerConnected())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			sceneManager_->ChangeScene("GamePlayScene");
		}
	}

	ImGui::Begin("GameTitleScene");
	ImGui::Text("A : GamePlayScene");
	ImGui::End();
}

void GameTitleScene::Draw()
{

}

void GameTitleScene::DrawUI()
{

}