#include "SampleScene.h"

void SampleScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//カメラの作成
	camera_ = CameraManager::CreateCamera("SampleCamera");
	camera_->translation_ = { 0.0f,8.3f,-20.0f };
	camera_->rotation_ = { 0.3f,0.0f,0.0f };

	//Terrainの作成
	terrainModel_ = ModelManager::CreateFromModelFile("terrain", Opaque);
	terrainWorldTransform_.Initialize();

	//MonsterBallの作成
	monsterballModel_ = ModelManager::CreateFromModelFile("Player", Opaque);
	monsterballWorldTransform_.Initialize();
	monsterballWorldTransform_.translation_ = { 0.0f,1.0f,0.0f };
}

void SampleScene::Finalize()
{
}

void SampleScene::Update()
{
	//TerrainWorldTransformの更新
	terrainWorldTransform_.UpdateMatrix();

	//MonsterBallWorldTransformの更新
	monsterballWorldTransform_.UpdateMatrix();
	monsterballModel_->Update(monsterballWorldTransform_, "Armature|mixamo.com|Layer0");

	//カメラの更新
	camera_->UpdateMatrix();

	//ImGui
	ImGui::Begin("SampleScene");
	ImGui::DragFloat3("CameraTranslation", &camera_->translation_.x, 0.01f);
	ImGui::DragFloat3("CameraRotation", &camera_->rotation_.x, 0.01f);
	ImGui::DragFloat3("TerrainTranslation", &terrainWorldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("TerrainRotation", &terrainWorldTransform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("TerrainScale", &terrainWorldTransform_.scale_.x, 0.01f);
	ImGui::DragFloat3("MonsterBallTranslation", &monsterballWorldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("MonsterBallRotation", &monsterballWorldTransform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("MonsterBallScale", &monsterballWorldTransform_.scale_.x, 0.01f);
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
	//Terrainの描画
	terrainModel_->Draw(terrainWorldTransform_, *camera_);

	//Monsterballの描画
	monsterballModel_->Draw(monsterballWorldTransform_, *camera_);

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
