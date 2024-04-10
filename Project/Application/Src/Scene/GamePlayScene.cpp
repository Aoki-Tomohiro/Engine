#include "GamePlayScene.h"

void GamePlayScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//カメラの初期化
	camera_.Initialize();
	//モデルの生成
	model_.reset(ModelManager::CreateFromOBJ("Cube", Opaque));
	//InputHandlerの生成
	inputHandler_ = std::make_unique<InputHandler>();
	//コマンドを割り当てる
	inputHandler_->AssingMoveLeftCommand2PressKeyA();
	inputHandler_->AssingMoveRightCommand2PressKeyD();
	//プレイヤーの生成
	player_ = std::make_unique<Player>();
	player_->Initialize(model_.get());
}

void GamePlayScene::Finalize()
{

}

void GamePlayScene::Update()
{
	//キーを取得
	command_ = inputHandler_->HandleInput();

	//コマンドを実行
	if (this->command_) {
		command_->Exec(*player_);
	}

	//プレイヤーの更新
	player_->Update();

	//カメラの更新
	camera_.UpdateMatrix();

	ImGui::Begin("GameScene");
	ImGui::Text("AD : Move");
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
	//プレイヤーの描画
	player_->Draw(camera_);

	//3Dオブジェクト描画
	renderer_->Render();
#pragma endregion

#pragma region パーティクル描画
	//パーティクル描画前処理
	renderer_->PreDrawParticles();

	//パーティクル描画後処理
	renderer_->PostDrawParticles();
#pragma endregion
}

void GamePlayScene::DrawUI()
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}