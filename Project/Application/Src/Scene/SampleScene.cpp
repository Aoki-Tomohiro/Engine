/**
 * @file SampleScene.cpp
 * @brief サンプルシーンを管理するファイル
 * @author 青木智滉
 * @date
 */

#include "SampleScene.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/Components/Model/ModelComponent.h"

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
	TextureManager::Load("Test1.dds");

	//テスト用オブジェクトを生成
	testObject_ = gameObjectManager_->CreateGameObject("GameObject");

	//トランスフォームの初期化
	testObjectTransform_ = testObject_->GetComponent<TransformComponent>();
	testObjectTransform_->worldTransform_.translation_.y = 10.0f;

	//モデル読み込み
	Model* model = ModelManager::CreateFromModelFile("Cube", Opaque);
	model->GetMaterial(1)->SetTexture("Test1.dds");
	model->SetCastShadows(false);

	//モデルコンポーネントの追加
	ModelComponent* modelComponent = testObject_->AddComponent<ModelComponent>();
	modelComponent->SetModel(model);

	//Skyboxの作成
	skybox_.reset(Skybox::Create());
	skybox_->SetScale({ 100.0f,100.0f,100.0f });
}

void SampleScene::Finalize()
{
}

void SampleScene::Update()
{
	//テストオブジェクトを回転させる
	testObjectTransform_->worldTransform_.rotation_.y += 0.02f;

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

	//スカイボックスの描画
	skybox_->Draw(*camera_);

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
