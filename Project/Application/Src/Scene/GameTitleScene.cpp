#include "GameTitleScene.h"

void GameTitleScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	camera_.Initialize();

	worldTransform_.Initialize();

	model_.reset(ModelManager::CreateFromModelFile("AnimatedCube.gltf", Opaque));
}

void GameTitleScene::Finalize()
{

}

void GameTitleScene::Update() 
{
	worldTransform_.UpdateMatrixFromEuler();

	camera_.UpdateMatrix();

	model_->ApplyAnimation();

	model_->Update(worldTransform_);

	ImGui::Begin("GameTitleScene");
	ImGui::DragFloat3("WorldTransform.translation", &worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("WorldTransform.rotation", &worldTransform_.rotation_.x, 0.1f);
	ImGui::DragFloat3("WorldTransform.scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("Camera.translation", &camera_.translation_.x, 0.1f);
	ImGui::DragFloat3("Camera.rotation", &camera_.rotation_.x, 0.1f);
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