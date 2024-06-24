#include "GamePlayScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/LevelLoader/LevelLoader.h"

void GamePlayScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//GameObjectManagerのインスタンスを取得
	gameObjectManager_ = GameObjectManager::GetInstance();

	//GameObjectのクリア
	gameObjectManager_->Clear();

	//LevelDataの読み込み
	LevelLoader::Load("GameScene");

	//カメラを取得
	camera_ = gameObjectManager_->GetCamera();

	//FollowCameraの作成
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	followCamera_->SetTarget(&gameObjectManager_->GetGameObject<Player>()->GetComponent<TransformComponent>()->worldTransform_);

	//プレイヤーの初期化
	Player* player = gameObjectManager_->GetGameObject<Player>();
	player->GetComponent<TransformComponent>()->worldTransform_.rotationType_ = RotationType::Quaternion;
	player->SetCamera(camera_);

	//トランジションの生成
	transition_ = std::make_unique<Transition>();
	transition_->Initialize();
}

void GamePlayScene::Finalize()
{

}

void GamePlayScene::Update()
{
	//GameObjectManagerの更新
	gameObjectManager_->Update();

	//トランジションの更新
	transition_->Update();

	//FollowCameraの更新
	followCamera_->Update();

	//カメラの更新
	*camera_ = followCamera_->GetCamera();
	camera_->UpdateMatrix();

	//フェードイン処理
	HandleTransition();

	//ImGui
	ImGui::Begin("GamePlayScene");
	ImGui::Text("K : GameClearScene");
	ImGui::Text("L : GameOverScene");
	ImGui::End();
}

void GamePlayScene::Draw()
{
#pragma region 背景スプライト描画
	//背景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//背景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion

	//深度バッファをクリア
	renderer_->ClearDepthBuffer();

#pragma region 3Dオブジェクト描画
	//GameObjectManagerの描画
	gameObjectManager_->Draw();

	//3Dオブジェクト描画
	renderer_->Render();
#pragma endregion
}

void GamePlayScene::DrawUI()
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//トランジションの描画
	transition_->Draw();

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void GamePlayScene::HandleTransition()
{
	//FadeInしていないとき
	if (transition_->GetFadeState() != transition_->FadeState::In)
	{
		//Kキーを押したらGameClearSceneに遷移
		if (input_->IsPushKeyEnter(DIK_K))
		{
			transition_->SetFadeState(Transition::FadeState::In);
			nextScene_ = kGameClearScene;
		}
		//Lキーを押したらGameOverSceneに遷移
		else if (input_->IsPushKeyEnter(DIK_L))
		{
			transition_->SetFadeState(Transition::FadeState::In);
			nextScene_ = kGameClearScene;
		}
	}

	//シーン遷移
	if (transition_->GetFadeInComplete())
	{
		switch (nextScene_)
		{
		case kGameClearScene:
			sceneManager_->ChangeScene("GameClearScene");
			break;
		case kGameOverScene:
			sceneManager_->ChangeScene("GameOverScene");
			break;
		}
	}
}