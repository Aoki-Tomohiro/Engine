#include "GamePlayScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Utilities/RandomGenerator.h"

void GamePlayScene::Initialize()
{
	input_ = Input::GetInstance();

	renderer_ = Renderer::GetInstance();

	gameObjectManager_ = GameObjectManager::GetInstance();
	gameObjectManager_->Initialize();

	collisionManager_ = std::make_unique<CollisionManager>();

	camera_.Initialize();

	playerModel_ = ModelManager::CreateFromOBJ("Cube", Opaque);
	player_ = GameObjectManager::CreateGameObject<Player>();
	player_->SetModel(playerModel_);
	player_->SetCamera(&camera_);

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	followCamera_->SetTarget(&player_->GetWorldTransform());

	groundSurfaceModel_ = ModelManager::CreateFromOBJ("Ground", Opaque);
	groundSurfaceModel_->SetUVScale({ 25.0f,25.0f });
	groundModel_ = ModelManager::CreateFromOBJ("Cube", Opaque);
	groundModel_->SetColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	ground_ = GameObjectManager::CreateGameObject<Ground>();
	ground_->SetModel(groundModel_);
	ground_->SetGroundSurfaceModel(groundSurfaceModel_);

	skydomeModel_ = ModelManager::CreateFromOBJ("Skydome", Opaque);
	skydome_ = GameObjectManager::CreateGameObject<Skydome>();
	skydome_->SetModel(skydomeModel_);

	bossModel_ = ModelManager::CreateFromOBJ("Cube", Opaque);
	bossModel_->SetColor({ 1.0f,0.0f,1.0f,1.0f });
	boss_ = GameObjectManager::CreateGameObject<Boss>();
	boss_->SetModel(bossModel_);
	boss_->SetTag("Boss");
}

void GamePlayScene::Finalize()
{

}

void GamePlayScene::Update()
{
	//カメラシェイクのフラグを立てる
	if (player_->GetWeapon()->GetIsHit())
	{
		cameraShakeEnable_ = true;
	}

	//ヒットストップのフラグを立てる
	if (!isStop_ && player_->GetWeapon()->GetIsHit())
	{
		isStop_ = true;
		player_->GetWeapon()->SetIsHit(false);
	}

	//カメラの処理
	camera_ = followCamera_->GetCamera();
	//カメラシェイクの処理
	if (cameraShakeEnable_)
	{
		if (++shakeTimer_ >= kShakeTime)
		{
			cameraShakeEnable_ = false;
			shakeTimer_ = 0;
		}

		if (player_->GetComboIndex() == 5)
		{
			camera_.translation_.y += RandomGenerator::GetRandomFloat(-0.6f, 0.6f);
		}
		else
		{
			camera_.translation_.y += RandomGenerator::GetRandomFloat(-0.1f, 0.1f);
		}
	}
	//行列の更新
	camera_.UpdateMatrix();

	//パーティクルの更新
	player_->UpdateParticle();

	//ImGuiの更新
	UpdateImGui();

	//ヒットストップの処理
	if (isStop_)
	{
		if (++stopTimer_ >= kStopTime)
		{
			isStop_ = false;
			stopTimer_ = 0;
		}
		return;
	}

	//ゲームオブジェクトの更新
	gameObjectManager_->Update();

	//追従カメラの更新
	followCamera_->Update();

	//衝突判定
	collisionManager_->ClearColliderList();
	collisionManager_->SetColliderList(player_);
	Weapon* weapon = player_->GetWeapon();
	if (weapon->GetIsAttack())
	{
		collisionManager_->SetColliderList(weapon);
	}
	collisionManager_->SetColliderList(boss_);
	collisionManager_->CheckAllCollisions();

	//シーン切り替え
	if (input_->IsPushKeyEnter(DIK_F1))
	{
		sceneManager_->ChangeScene("GameClearScene");
	}

	if (input_->IsPushKeyEnter(DIK_F2))
	{
		sceneManager_->ChangeScene("GameOverScene");
	}
}

void GamePlayScene::Draw()
{
#pragma region モデルの描画

	renderer_->PreDrawModels();

	gameObjectManager_->Draw(camera_);

	renderer_->PostDrawModels();

#pragma endregion

#pragma region パーティクルの描画

	renderer_->PreDrawParticles();

	player_->DrawParticle(camera_);

	renderer_->PostDrawParticles();

#pragma endregion
}

void GamePlayScene::DrawUI()
{
#pragma region スプライトの描画

	renderer_->PreDrawSprites(Renderer::kBlendModeNormal);

	renderer_->PostDrawSprites();

#pragma endregion
}

void GamePlayScene::UpdateImGui()
{
	player_->UpdateImGui();
	ImGui::Begin("GamePlayScene");
	ImGui::Text("F1 : GameClearScene");
	ImGui::Text("F2 : GameOverScene");
	ImGui::End();
}