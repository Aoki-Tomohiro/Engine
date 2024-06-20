#include "GameTitleScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Components/PostEffects/PostEffects.h"

void GameTitleScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//カメラの初期化
	camera_.Initialize();
	camera_.translation_ = { 0.0f,12.3f,-50.0f };
	camera_.rotation_ = { 0.157f,0.0f,0.0f };

	//プレイヤーの生成
	player_ = std::make_unique<Player>();
	player_->Initialize();

	//敵の生成
	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize();

	//CollisionManagerの作成
	collisionManager_ = std::make_unique<CollisionManager>();

	outlineData.isEnable = false;
	outlineData.projectionInverse = Mathf::Inverse(camera_.matProjection_);
	outlineData.coefficient = 1.0f;
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
	outlineData.projectionInverse = Mathf::Inverse(camera_.matProjection_);

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

	//ImGui
	ImGui::Begin("GameTitleScene");
	ImGui::DragFloat3("CameraTranslation", &camera_.translation_.x, 0.1f);
	ImGui::DragFloat3("CameraRotation", &camera_.rotation_.x, 0.01f);
	ImGui::DragInt("IsOutlineEnable", &outlineData.isEnable, 1, 0, 1);
	ImGui::DragFloat("Coefficient", &outlineData.coefficient, 0.1f);
	ImGui::End();

	//PostEffects
	PostEffects* postEffects = PostEffects::GetInstance();
	postEffects->SetIsEnable(false);
	postEffects->GetOutline()->SetIsEnable(outlineData.isEnable);
	postEffects->GetOutline()->SetProjectionInverse(outlineData.projectionInverse);
	postEffects->GetOutline()->SetCoefficient(outlineData.coefficient);
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