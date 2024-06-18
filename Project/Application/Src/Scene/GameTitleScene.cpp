#include "GameTitleScene.h"
#include "Engine/Framework/Scene/SceneManager.h"

void GameTitleScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//カメラの初期化
	camera_.Initialize();

	//プレイヤーの生成
	player_ = std::make_unique<Player>();
	player_->Initialize();

	//敵の生成
	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize();

	//CollisionManagerの作成
	collisionManager_ = std::make_unique<CollisionManager>();
}

void GameTitleScene::Finalize()
{

}

void GameTitleScene::Update()
{
	//プレイヤーの更新
	player_->Update();

	//敵の更新
	enemy_->Update();

	//カメラの更新
	camera_.UpdateMatrix();

	//衝突判定
	collisionManager_->ClearColliderList();
	if (Collider* collider = player_->GetComponent<Collider>())
	{
		collisionManager_->SetColliderList(collider);
	}
	if (Collider* collider = enemy_->GetComponent<Collider>())
	{
		collisionManager_->SetColliderList(collider);
	}
	collisionManager_->CheckAllCollisions();
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
	//プレイヤーの描画
	player_->Draw(camera_);

	//敵の描画
	enemy_->Draw(camera_);

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