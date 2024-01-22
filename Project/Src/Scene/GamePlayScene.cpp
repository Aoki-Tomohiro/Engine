#include "GamePlayScene.h"
#include "Engine/Framework/Scene/SceneManager.h"

void GamePlayScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	camera_.Initialize();

	//追従カメラの生成
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();

	//プレイヤーの生成
	playerModel_.reset(ModelManager::CreateFromOBJ("Player", Opaque));
	playerModel_->SetColor({ 0.5f, 1.0f, 0.7f, 1.0f });
	playerModel_->SetEnableLighting(false);
	player_ = GameObjectManager::CreateGameObject<Player>();
	player_->SetTag("Player");
	player_->SetModel(playerModel_.get());
	player_->SetCamera(&camera_);
	followCamera_->SetTarget(&player_->GetWorldTransform());

	//天球の生成
	skydomeModel_.reset(ModelManager::CreateFromOBJ("Skydome", Opaque));
	skydomeModel_->SetEnableLighting(false);
	skydome_ = GameObjectManager::CreateGameObject<Skydome>();
	skydome_->SetModel(skydomeModel_.get());

	//地面の生成
	groundModel_.reset(ModelManager::CreateFromOBJ("Cube", Opaque));
	groundModel_->SetColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	groundModel_->SetEnableLighting(false);
	ground_ = GameObjectManager::CreateGameObject<Ground>();
	ground_->SetModel(groundModel_.get());
}

void GamePlayScene::Finalize()
{

}

void GamePlayScene::Update()
{
	//ゲームオブジェクトの更新
	GameObjectManager::GetInstance()->Update();

	//パーティクルの更新
	ParticleManager::GetInstance()->Update();

	//カメラの更新
	followCamera_->Update();
	camera_ = followCamera_->GetCamera();
	camera_.UpdateMatrix();
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
	//ゲームオブジェクトの描画
	GameObjectManager::GetInstance()->Draw(camera_);

	//3Dオブジェクト描画
	renderer_->Render();
#pragma endregion

#pragma region パーティクル描画
	//パーティクル描画前処理
	renderer_->PreDrawParticles();

	//パーティクルの描画
	ParticleManager::GetInstance()->Draw(camera_);

	//パーティクル描画後処理
	renderer_->PostDrawParticles();
#pragma endregion
}

void GamePlayScene::DrawUI()
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//ゲームオブジェクトのUIの描画
	GameObjectManager::GetInstance()->DrawUI();

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}