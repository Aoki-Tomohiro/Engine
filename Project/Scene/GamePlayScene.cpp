#include "GamePlayScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Utilities/RandomGenerator.h"
#include "Engine/Math/MathFunction.h"

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
	player_->SetTag("Player");
	player_->SetModel(playerModel_);
	player_->SetCamera(&camera_);

	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();
	player_->SetLockOn(lockOn_.get());

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	followCamera_->SetTarget(&player_->GetWorldTransform());
	followCamera_->SetLockOn(lockOn_.get());

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

	//スプライトの生成
	sprite_.reset(Sprite::Create("Project/Resources/Images/white.png", { 0.0f,0.0f }));
	sprite_->SetSize({ 1280.0f,720.0f });
	sprite_->SetColor(spriteColor_);
}

void GamePlayScene::Finalize()
{

}

void GamePlayScene::Update()
{
	//トランジションの処理
	if (!isTransitionEnd_)
	{
		transitionTimer_ += 1.0f / kTransitionTime;
		spriteColor_.w = Mathf::Lerp(spriteColor_.w, 0.0f, transitionTimer_);
		sprite_->SetColor(spriteColor_);

		if (spriteColor_.w <= 0.0f)
		{
			isTransitionEnd_ = true;
			transitionTimer_ = 0.0f;
		}
	}

	if (isTransition_)
	{
		transitionTimer_ += 1.0f / kTransitionTime;
		spriteColor_.w = Mathf::Lerp(spriteColor_.w, 1.0f, transitionTimer_);
		sprite_->SetColor(spriteColor_);

		if (spriteColor_.w >= 1.0f)
		{
			switch (nextScene)
			{
			case GameClearScene:
				sceneManager_->ChangeScene("GameClearScene");
				break;
			case GameOverScene:
				sceneManager_->ChangeScene("GameOverScene");
				break;
			}
		}
	}

	//カメラシェイクのフラグを立てる
	if (player_->GetWeapon()->GetIsHit() || player_->GetIsHit())
	{
		cameraShakeEnable_ = true;

		if (player_->GetComboIndex() == 3)
		{
			shakeIntensityX = 0.0f;
			shakeIntensityY = 0.6f;
		}
		else if (player_->GetIsHit())
		{
			shakeIntensityX = 0.6f;
			shakeIntensityY = 0.6f;
		}
		else
		{
			shakeIntensityX = 0.0f;
			shakeIntensityY = 0.1f;
		}
	}

	//ヒットストップのフラグを立てる
	if (!isStop_ && player_->GetWeapon()->GetIsHit())
	{
		isStop_ = true;
		if (player_->GetComboIndex() == 3)
		{
			kStopTime = 10;
		}
		else
		{
			kStopTime = 2;
		}
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

		camera_.translation_.x += RandomGenerator::GetRandomFloat(-shakeIntensityX, shakeIntensityX);
		camera_.translation_.y += RandomGenerator::GetRandomFloat(-shakeIntensityY, shakeIntensityY);
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

	//ロックオンの処理
	lockOn_->Update(boss_, camera_);

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
	for (const std::unique_ptr<Missile>& missile : boss_->GetMissiles())
	{
		collisionManager_->SetColliderList(missile.get());
	}
	for (const std::unique_ptr<Laser>& laser : boss_->GetLasers())
	{
		collisionManager_->SetColliderList(laser.get());
	}
	collisionManager_->CheckAllCollisions();

	////ゲームクリア処理
	//if (boss_->GetHP() <= 0.0f)
	//{
	//	nextScene = GameClearScene;
	//	isTransition_ = true;
	//}

	////ゲームオーバー処理
	//if (player_->GetHP() <= 0.0f)
	//{
	//	nextScene = GameOverScene;
	//	isTransition_ = true;
	//}

	//シーン切り替え
	if (isTransitionEnd_)
	{
		if (input_->IsPushKeyEnter(DIK_F1))
		{
			nextScene = GameClearScene;
			isTransition_ = true;
		}

		if (input_->IsPushKeyEnter(DIK_F2))
		{
			nextScene = GameOverScene;
			isTransition_ = true;
		}
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

	player_->DrawUI();

	boss_->DrawUI();

	lockOn_->Draw();

	sprite_->Draw();

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