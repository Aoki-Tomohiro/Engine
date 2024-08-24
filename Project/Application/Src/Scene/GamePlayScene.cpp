#include "GamePlayScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Components/Model/ModelComponent.h"

void GamePlayScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//ゲームオブジェクトマネージャーの初期化
	gameObjectManager_ = GameObjectManager::GetInstance();
	gameObjectManager_->Clear();

	//パーティクルマネージャーを初期化
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Clear();

	////レベルデータの読み込み
	//LevelLoader::Load("GameScene");

	//カメラを設定
	camera_ = CameraManager::GetInstance()->GetCamera("Camera");
	gameObjectManager_->SetCamera(camera_);
	particleManager_->SetCamera(camera_);

	//衝突マネージャーの生成
	collisionManager_ = std::make_unique<CollisionManager>();

	//トランジションの生成
	transition_ = std::make_unique<Transition>();
	transition_->Initialize();
}

void GamePlayScene::Finalize()
{

}

void GamePlayScene::Update()
{
	//トランジションの更新
	transition_->Update();

	//ゲームオブジェクトマネージャーの更新
	gameObjectManager_->Update();

	//コライダーをクリア
	collisionManager_->ClearColliderList();
	//衝突判定
	collisionManager_->CheckAllCollisions();

	//シャドウマップ用のカメラを移動させる
	Camera* shadowCamera = CameraManager::GetCamera("ShadowCamera");
	shadowCamera->translation_ = { camera_->translation_.x,shadowCamera->translation_.y,camera_->translation_.z };

	//カメラの更新
	camera_->UpdateMatrix();
}

void GamePlayScene::Draw()
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
	//ゲームオブジェクトマネージャーの描画
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