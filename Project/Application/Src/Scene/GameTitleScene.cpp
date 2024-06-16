#include "GameTitleScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"

void GameTitleScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	model_.reset(ModelManager::CreateFromModelFile("BrainStem", Opaque));

	worldTransform_.Initialize();

	camera_.Initialize();
}

void GameTitleScene::Finalize()
{

}

void GameTitleScene::Update()
{
	worldTransform_.UpdateMatrixFromEuler();

	camera_.UpdateMatrix();

	model_->Update(worldTransform_, "Dance");

	ImGui::Begin("GameTitleScene");
	ImGui::DragFloat3("CameraTranslation", &camera_.translation_.x, 0.01f);
	ImGui::DragFloat3("CameraRotation", &camera_.rotation_.x, 0.01f);
	ImGui::End();
}

void GameTitleScene::Draw()
{
#pragma region 背景スプライト描画
	//背景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//背景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion

	//深度バッファをクリア
	renderer_->ClearDepthBuffer();

#pragma region Skyboxの描画
	//Skybox描画前処理
	renderer_->PreDrawSkybox();

	//Skybox描画処理
	renderer_->PostDrawSkybox();
#pragma endregion

#pragma region 3Dオブジェクト描画
	model_->Draw(worldTransform_, camera_);

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

void GameTitleScene::DrawUI()
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}