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

	//パーティクルマネージャーの初期化
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Clear();
	particleManager_->SetCamera(camera_);

	//パーティクルシステムの初期化
	particleSystem_ = ParticleManager::Create("Rock");
	particleSystem_->SetModel("Rock");
	particleSystem_->SetBlendMode(BlendMode::kBlendModeNormal);
	particleSystem_->SetIsBillboard(false);
	particleSystem_->SetEnableDepthWrite(true);

	//エミッターの生成
	ParticleEmitter* emitter = EmitterBuilder()
		.SetEmitterName("Rock").SetColor({ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f })
		.SetEmitterLifeTime(1000.0f).SetCount(10).SetFrequency(0.1f).SetLifeTime(0.8f, 1.0f).SetRadius(1.0f)
		.SetRotate({ 0.0f,0.0f,0.0f }, { 6.28f,6.28f,6.28f }).SetScale({ 1.6f, 1.6f, 1.6f }, { 2.0f, 2.0f, 2.0f })
		.SetTranslation({ 0.0f,0.0f,-4.0f }).SetVelocity({ -0.06f, 0.06f, -0.06f }, { 0.06f, 0.12f, 0.06f }).Build();

	//パーティクルシステムにエミッターを追加
	particleSystem_->AddParticleEmitter(emitter);
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

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}
