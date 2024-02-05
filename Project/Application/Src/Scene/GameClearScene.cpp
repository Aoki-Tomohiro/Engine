#include "GameClearScene.h"
#include "Engine/Framework/Scene/SceneManager.h"

void GameClearScene::Initialize() 
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//テクスチャの読み込み
	TextureManager::Load("circle.png");

	//カメラの初期化
	camera_.Initialize();

	//パーティクルをクリア
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Clear();

	//パーティクルシステムの生成
	particleSystem_ = ParticleManager::Create("Test");
	particleSystem_->SetTexture("circle.png");
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
			sceneManager_->ChangeScene("GameTitleScene");
		}
	}

	if (input_->IsPushKeyEnter(DIK_SPACE))
	{
		sceneManager_->ChangeScene("GameTitleScene");
	}

	if (input_->IsControllerConnected())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
		{
			ParticleEmitter* newEmitter = ParticleEmitterBuilder()
				.SetDeleteTime(1.0f)
				.SetEmitterName("Test")
				.SetPopAzimuth(0.0f,360.0f)
				.SetPopColor({0.0f,0.0f,0.0f,1.0f},{1.0f,1.0f,1.0f,1.0f})
				.SetPopCount(10)
				.SetPopElevation(0.0f,0.0f)
				.SetPopFrequency(0.1f)
				.SetPopLifeTime(0.2f,0.4f)
				.SetPopScale({0.2f,0.2f,0.2f},{0.4f,0.4f,0.4f})
				.SetPopVelocity({ 0.06f,0.06f,0.06f },{ 0.1f,0.1f,0.1f })
				.SetTranslation({0.0f,0.0f,0.0f})
				.Build();
			particleSystem_->AddParticleEmitter(newEmitter);
		}
	}

	//パーティクルの更新
	particleManager_->Update();

	ImGui::Begin("GameClearScene");
	ImGui::Text("Space or AButton : GameTitleScene");
	ImGui::End();
}

void GameClearScene::Draw() 
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
	//3Dオブジェクト描画
	renderer_->Render();
#pragma endregion

#pragma region パーティクル描画
	//パーティクル描画前処理
	renderer_->PreDrawParticles();

	//パーティクルの描画
	particleManager_->Draw(camera_);

	//パーティクル描画後処理
	renderer_->PostDrawParticles();
#pragma endregion
}

void GameClearScene::DrawUI() 
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}