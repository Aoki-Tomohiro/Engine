#include "GamePlayScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/LevelLoader/LevelLoader.h"
#include "Engine/Utilities/GameTimer.h"
#include "Engine/Utilities/RandomGenerator.h"
#include "Engine/Utilities/GlobalVariables.h"

void GamePlayScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//カメラの初期化
	camera_.Initialize();

	//GameObjectManagerの初期化
	gameObjectManager_ = GameObjectManager::GetInstance();
	gameObjectManager_->Clear();
	gameObjectManager_->SetCamera(&camera_);

	//ParticleManagerを初期化
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Clear();
	particleManager_->SetCamera(&camera_);

	//LevelDataの読み込み
	LevelLoader::Load("GameScene");

	//LockOnの生成
	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();

	//FollowCameraの作成
	cameraController_ = std::make_unique<CameraController>();
	cameraController_->Initialize();
	cameraController_->SetTarget(&gameObjectManager_->GetGameObject<Player>()->GetComponent<TransformComponent>()->worldTransform_);
	cameraController_->SetLockOn(lockOn_.get());

	//プレイヤーの初期化
	Player* player = gameObjectManager_->GetGameObject<Player>();
	//TransformComponentの初期化
	TransformComponent* playerTransformComponent = player->GetComponent<TransformComponent>();
	//Colliderの設定
	playerTransformComponent->worldTransform_.rotationType_ = RotationType::Quaternion;
	AABBCollider* playerCollider = player->GetComponent<AABBCollider>();
	playerCollider->SetCollisionAttribute(kCollisionAttributePlayer);
	playerCollider->SetCollisionMask(kCollisionMaskPlayer);
	//カメラを設定
	player->SetCamera(&camera_);
	//LockOnを設定
	player->SetLockOn(lockOn_.get());

	//敵の初期化
	Enemy* enemy = gameObjectManager_->GetGameObject<Enemy>();
	//TransformComponentの初期化
	TransformComponent* enemyTransformComponent = enemy->GetComponent<TransformComponent>();
	enemyTransformComponent->worldTransform_.rotationType_ = RotationType::Quaternion;
	//Colliderの設定
	AABBCollider* enemyCollider = enemy->GetComponent<AABBCollider>();
	enemyCollider->SetCollisionAttribute(kCollisionAttributeEnemy);
	enemyCollider->SetCollisionMask(kCollisionMaskEnemy);

	//武器の生成
	Weapon* weapon = gameObjectManager_->CreateGameObject<Weapon>();
	//TransformComponentの追加
	TransformComponent* weaponTransformComponent = weapon->AddComponent<TransformComponent>();
	weaponTransformComponent->Initialize();
	weaponTransformComponent->worldTransform_.translation_ = { 0.0f,4.0f,0.0f };
	weaponTransformComponent->worldTransform_.scale_ = { 1.2f,1.8f,1.2f };
	//ModelComponentの追加
	ModelComponent* weaponModelComponent = weapon->AddComponent<ModelComponent>();
	weaponModelComponent->Initialize("Sword", Opaque);
	weaponModelComponent->SetTransformComponent(weaponTransformComponent);
	//ColliderComponentの追加
	OBBCollider* weaponCollider = weapon->AddComponent<OBBCollider>();
	weaponCollider->SetCollisionAttribute(kCollisionAttributeWeapon);
	weaponCollider->SetCollisionMask(kCollisionMaskWeapon);
	//プレイヤーを親に設定
	weapon->SetParent(playerTransformComponent);
	//描画しないようにする
	weapon->SetIsVisible(false);

	//トランジションの生成
	transition_ = std::make_unique<Transition>();
	transition_->Initialize();

	//CollisionManagerの生成
	collisionManager_ = std::make_unique<CollisionManager>();

	//Skyboxの初期化
	skybox_.reset(Skybox::Create());

	//ゲームオーバーのスプライトの生成
	TextureManager::Load("GameOver.png");
	gameOverSprite_.reset(Sprite::Create("GameOver.png", { 0.0f,0.0f }));

	//ゲームクリアのスプライトの生成
	TextureManager::Load("GameClear.png");
	gameClearSprite_.reset(Sprite::Create("GameClear.png", { 0.0f,0.0f }));

	//環境変数の設定
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "GameScene";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "HitStopDuration", hitStopSettings_.duration);
}

void GamePlayScene::Finalize()
{

}

void GamePlayScene::Update()
{
	//ヒットストップの処理
	UpdateHitStop();

	//パリィの処理
	UpdateParry();

	//GameObjectManagerの更新
	gameObjectManager_->Update();

	//トランジションの更新
	transition_->Update();

	//ロックオンの処理
	Enemy* enemy = gameObjectManager_->GetGameObject<Enemy>();
	lockOn_->SetTargetOffset(enemy->GetColliderOffset());
	lockOn_->Update(enemy, camera_);

	//CameraControllerの更新
	cameraController_->Update();

	//カメラの更新
	camera_ = cameraController_->GetCamera();
	camera_.UpdateMatrix();

	//フェードイン処理
	HandleTransition();

	//衝突判定
	collisionManager_->ClearColliderList();
	if (Collider* collider = gameObjectManager_->GetGameObject<Player>()->GetComponent<Collider>())
	{
		collisionManager_->SetColliderList(collider);
	}
	if (Collider* collider = enemy->GetComponent<Collider>())
	{
		collisionManager_->SetColliderList(collider);
	}
	Weapon* weapon = gameObjectManager_->GetGameObject<Weapon>();
	if (weapon->GetIsVisible())
	{
		if (Collider* collider = weapon->GetComponent<Collider>())
		{
			collisionManager_->SetColliderList(collider);
		}
	}
	collisionManager_->CheckAllCollisions();

	//グローバル変数の適用
	ApplyGlobalVariables();

	//ImGui
	ImGui::Begin("GamePlayScene");
	ImGui::Text("K : GameClearScene");
	ImGui::Text("L : GameOverScene");
	ImGui::End();
}

void GamePlayScene::Draw()
{
#pragma region Skybox描画
	//Skybox描画前処理
	renderer_->PreDrawSkybox();

	//Skyboxの描画
	skybox_->Draw(camera_);

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
	//GameObjectManagerの描画
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

	//GameObjectManagerのUIを描画
	gameObjectManager_->DrawUI();

	//ロックオンの描画
	lockOn_->Draw();

	//ゲームオーバーの表示
	if (isGameClear_)
	{
		gameClearSprite_->Draw();
	}
	else if (isGameOver_)
	{
		gameOverSprite_->Draw();
	}

	//トランジションの描画
	transition_->Draw();

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void GamePlayScene::HandleTransition()
{
	//FadeInしていないとき
	if (transition_->GetFadeState() != transition_->FadeState::In)
	{
		Player* player = gameObjectManager_->GetGameObject<Player>();
		Enemy* enemy = gameObjectManager_->GetGameObject<Enemy>();
		//Kキーを押したらGameClearSceneに遷移
		if (input_->IsPushKeyEnter(DIK_K) || enemy->GetHP() <= 0.0f)
		{
			//transition_->SetFadeState(Transition::FadeState::In);
			//nextScene_ = kGameClearScene;
			isGameClear_ = true;
			player->SetIsInTitleScene(true);
			enemy->SetIsInTitleScene(true);
			ModelComponent* modelComponent = enemy->GetComponent<ModelComponent>();
			modelComponent->SetAnimationName("Idle");
		}
		//Lキーを押したらGameOverSceneに遷移
		else if (input_->IsPushKeyEnter(DIK_L) || player->GetHP() <= 0.0f)
		{
			//transition_->SetFadeState(Transition::FadeState::In);
			//nextScene_ = kGameOverScene;
			isGameOver_ = true;
			player->SetIsInTitleScene(true);
			enemy->SetIsInTitleScene(true);
			ModelComponent* modelComponent = enemy->GetComponent<ModelComponent>();
			modelComponent->SetAnimationName("Idle");
		}
	}

	//ゲームクリアかゲームオーバーの時のどちらかになっていたらタイトルに戻る
	if (isGameClear_ || isGameOver_)
	{
		if (input_->IsPressButton(XINPUT_GAMEPAD_A))
		{
			transition_->SetFadeState(Transition::FadeState::In);
		}
	}

	//シーン遷移
	if (transition_->GetFadeInComplete())
	{
		sceneManager_->ChangeScene("GameTitleScene");
	}
}

void GamePlayScene::UpdateHitStop()
{
	//ヒットストップが有効ではない場合
	if (!hitStopSettings_.isActive)
	{
		//プレイヤーの攻撃がヒットしていたらヒットストップを有効化する
		if (gameObjectManager_->GetGameObject<Weapon>()->GetIsHit())
		{
			//ヒットストップを有効にする
			hitStopSettings_.isActive = true;
			gameObjectManager_->GetGameObject<Player>()->SetIsActive(false);
			gameObjectManager_->GetGameObject<Enemy>()->SetIsActive(false);
			gameObjectManager_->GetGameObject<Weapon>()->SetIsActive(false);
		}
	}

	//ヒットストップが有効の場合
	if (hitStopSettings_.isActive)
	{
		//タイマーを進める
		hitStopSettings_.timer += GameTimer::GetDeltaTime();

		//ヒットストップのタイマーが指定の時間を超えたら無効化する
		if (hitStopSettings_.timer > hitStopSettings_.duration)
		{
			hitStopSettings_.timer = 0.0f;
			hitStopSettings_.isActive = false;
			gameObjectManager_->GetGameObject<Player>()->SetIsActive(true);
			gameObjectManager_->GetGameObject<Enemy>()->SetIsActive(true);
			gameObjectManager_->GetGameObject<Weapon>()->SetIsActive(true);
		}
	}
}

void GamePlayScene::UpdateParry()
{
	//プレイヤーがパリィに成功していた場合
	if (gameObjectManager_->GetGameObject<Weapon>()->GetIsParrySuccessful())
	{
		//スロウモーションのフラグを立てる
		parrySettings_.isActive = true;

		//敵の行動を遅くする
		gameObjectManager_->GetGameObject<Enemy>()->SetTimeScale(0.2f);

		//カメラを近づける
		cameraController_->SetDestinationOffset({ 0.0f, 2.0f, -10.0f });

		//GrayScaleを有効化
		PostEffects::GetInstance()->GetGrayScale()->SetIsEnable(true);

		//ゲーム全体の時間の流れを遅くする
		GameTimer::SetTimeScale(0.2f);
	}

	//パリィ状態の時
	if (parrySettings_.isActive)
	{
		//タイマーを進める
		const float kParryDeltaTime = 1.0f / 60.0f;
		parrySettings_.timer += kParryDeltaTime;

		//停止時間を超えていたら
		if (parrySettings_.timer > parrySettings_.stopDuration)
		{
			//カメラの元に戻す
			cameraController_->SetDestinationOffset({ 0.0f, 2.0f, -20.0f });

			//ゲーム全体の時間の流れをもとに戻す
			GameTimer::SetTimeScale(1.0f);
		}

		if (parrySettings_.timer > parrySettings_.duration)
		{
			//タイマーのリセット
			parrySettings_.timer = 0.0f;

			//フラグのリセット
			parrySettings_.isActive = false;

			//敵の行動をもとに戻す
			gameObjectManager_->GetGameObject<Enemy>()->SetTimeScale(1.0f);

			//GrayScaleを無効化
			PostEffects::GetInstance()->GetGrayScale()->SetIsEnable(false);
		}
	}
}

void GamePlayScene::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "GameScene";
	hitStopSettings_.duration = globalVariables->GetFloatValue(groupName, "HitStopDuration");
}