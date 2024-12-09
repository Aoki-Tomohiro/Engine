#include "SampleScene.h"

void SampleScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//GameObjectの初期化
	gameObjectManager_ = GameObjectManager::GetInstance();
	gameObjectManager_->Clear();

	//LevelDataの読み込み
	LevelManager::LoadLevelAndCreateObjects("SampleScene");

	//カメラを取得
	camera_ = CameraManager::GetCamera("Camera");

	//テクスチャの読み込み
	TextureManager::Load("Test.dds");

	//スプライトの生成
	sprite_.reset(Sprite::Create("Test.dds", { 1280.0f / 2.0f, 720.0f / 2.0f }));
	sprite_->SetAnchorPoint({ 0.5f,0.5f });
}

void SampleScene::Finalize()
{
}

void SampleScene::Update()
{
	//GameObjectの更新
	gameObjectManager_->Update();

	//カメラの更新
	camera_->UpdateMatrix();

	//ImGui
	ImGui::Begin("SampleScene");
	ImGui::DragFloat3("CameraTranslation", &camera_->translation_.x, 0.01f);
	ImGui::DragFloat3("CameraRotation", &camera_->rotation_.x, 0.01f);
	ImGui::End();
}

void SampleScene::Draw()
{
#pragma region Skybox描画
	//Skybox描画前処理
	renderer_->PreDrawSkybox();

	//Skybox描画後処理
	renderer_->PostDrawSkybox();
#pragma endregion

#pragma region 背景スプライト描画
	//背景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//背景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion

	//深度バッファをクリア
	renderer_->ClearDepthBuffer();

#pragma region 3Dオブジェクト描画
	//GameObjectの描画
	gameObjectManager_->Draw(*camera_);

	//3Dオブジェクト描画
	renderer_->Render();
#pragma endregion
}

void SampleScene::DrawUI()
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//スプライトの描画
	sprite_->Draw();

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}