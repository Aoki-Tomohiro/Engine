#include "GamePlayScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Components/Collision/OBBCollider.h"
#include "Engine/Components/Collision/CollisionAttributeManager.h"
#include "Engine/Components/Animator/AnimatorComponent.h"

void GamePlayScene::Initialize()
{
	//レンダラーのインスタンスを取得
	renderer_ = Renderer::GetInstance();

	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//オーディオのインスタンスを取得
	audio_ = Audio::GetInstance();

	//ヒットストップの生成
	hitStop_ = std::make_unique<HitStop>();

	//トランジションの生成
	transition_ = std::make_unique<Transition>();
	transition_->Initialize();

	//ゲームオブジェクトマネージャー、パーティクルマネージャー、コリジョンマネージャーの初期化
	InitializeManagers();

	//カメラとロックオンシステムの初期化
	InitializeCameraAndLockon();

	//プレイヤーとの初期化
	InitializePlayer();

	//プレイヤーの武器の初期化
	InitializePlayerWeapon();

	//魔法の球体の初期化
	InitializeOrb();

	//敵の初期化
	InitializeEnemy();

	//敵の武器の初期化
	InitializeEnemyWeapon();

	//カメラコントローラーの初期化
	InitializeCameraController();

	//スカイボックスの初期化
	skybox_.reset(Skybox::Create());

	//ゲームオーバーのスプライトの生成
	TextureManager::Load("GameOver.png");
	gameOverSprite_.reset(Sprite::Create("GameOver.png", { 0.0f,0.0f }));

	//ゲームクリアのスプライトの生成
	TextureManager::Load("GameClear.png");
	gameClearSprite_.reset(Sprite::Create("GameClear.png", { 0.0f,0.0f }));

	//音声データの読み込みと再生
	audioHandle_ = audio_->LoadAudioFile("GameScene.mp3");
	audio_->PlayAudio(audioHandle_, true, 0.2f);
}

void GamePlayScene::Finalize()
{

}

void GamePlayScene::Update()
{
	//ゲームオブジェクトマネージャーの更新
	gameObjectManager_->Update();

	//コライダーをクリアし、必要なオブジェクトを追加
	UpdateColliders();

	//ヒット判定とカメラシェイクの処理
	HandleCameraShake();

	//カメラとロックオンの更新
	UpdateCameraAndLockOn();

	//ヒットストップの更新
	hitStop_->Update();

	//CombatAnimationEditorの更新
	combatAnimationEditor_->Update();

	//トランジションの更新
	transition_->Update();
	HandleTransition();
}

void GamePlayScene::Draw()
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

	//プレイヤーのUIの描画
	gameObjectManager_->DrawUI();

	//ロックオンの描画
	lockon_->Draw();

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

void GamePlayScene::InitializeManagers()
{
	//ゲームオブジェクトマネージャーとパーティクルマネージャーを初期化
	gameObjectManager_ = GameObjectManager::GetInstance();
	gameObjectManager_->Clear();

	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Clear();

	//コリジョンマネージャーの生成
	collisionManager_ = std::make_unique<CollisionManager>();

	//レベルデータの読み込み
	LevelLoader::Load("GameScene");
}

void GamePlayScene::InitializeCameraAndLockon()
{
	//カメラの設定
	camera_ = CameraManager::GetInstance()->GetCamera("Camera");
	gameObjectManager_->SetCamera(camera_);
	particleManager_->SetCamera(camera_);

	//CombatAnimationEditorとロックオンシステムの初期化
	combatAnimationEditor_ = std::make_unique<CombatAnimationEditor>();
	combatAnimationEditor_->Initialize();

	lockon_ = std::make_unique<Lockon>();
	lockon_->Initialize();
}

void GamePlayScene::InitializePlayer()
{
	//プレイヤーの初期化
	player_ = gameObjectManager_->GetMutableGameObject<Player>("");
	player_->SetIsInTitleScene(false);
	player_->SetCamera(camera_);
	player_->SetLockon(lockon_.get());
	player_->SetCombatAnimationEditor(combatAnimationEditor_.get());
}

void GamePlayScene::InitializePlayerWeapon()
{    
	//プレイヤーの武器の生成と設定
	playerWeapon_ = GameObjectManager::CreateGameObject<Weapon>();
	playerWeapon_->SetName("PlayerWeapon");
	playerWeapon_->SetHitStop(hitStop_.get());

	//トランスフォームの設定
	TransformComponent* weaponTransformComponent = playerWeapon_->GetComponent<TransformComponent>();
	weaponTransformComponent->worldTransform_.parent_ = &player_->GetComponent<ModelComponent>()->GetModel()->GetJointWorldTransform("mixamorig:RightHand");
}

void GamePlayScene::InitializeOrb()
{
	//魔法の球体の生成と設定
	Orb* orb = GameObjectManager::CreateGameObject<Orb>();
	orb->SetLockon(lockon_.get());
	orb->SetTarget(&player_->GetComponent<ModelComponent>()->GetModel()->GetJointWorldTransform("mixamorig:Hips"));
}

void GamePlayScene::InitializeEnemy()
{
	//敵の初期化
	enemy_ = gameObjectManager_->GetMutableGameObject<Enemy>("");
	enemy_->SetIsInTitleScene(false);
	enemy_->SetCombatAnimationEditor(combatAnimationEditor_.get());
}

void GamePlayScene::InitializeEnemyWeapon()
{
	//敵の武器の生成と設定
	enemyWeapon_ = GameObjectManager::CreateGameObject<Weapon>();
	enemyWeapon_->SetName("EnemyWeapon");
	enemyWeapon_->SetHitStop(hitStop_.get());

	//トランスフォームの設定
	TransformComponent* weaponTransformComponent = enemyWeapon_->GetComponent<TransformComponent>();
	weaponTransformComponent->worldTransform_.parent_ = &enemy_->GetComponent<ModelComponent>()->GetModel()->GetJointWorldTransform("mixamorig:RightHand");
}

void GamePlayScene::InitializeCameraController()
{
	//カメラパスマネージャーとカメラコントローラーの初期化
	cameraPathManager_ = std::make_unique<CameraPathManager>();
	cameraPathManager_->Initialize();

	cameraController_ = std::make_unique<CameraController>();
	cameraController_->Initialize();
	cameraController_->SetCameraPathManager(cameraPathManager_.get());
	cameraController_->SetLockon(lockon_.get());
	cameraController_->SetTarget(&player_->GetComponent<ModelComponent>()->GetModel()->GetJointWorldTransform("mixamorig:Hips"));
}

void GamePlayScene::UpdateColliders()
{
	//コライダーをクリア
	collisionManager_->ClearColliderList();

	//プレイヤーを衝突マネージャーに追加
	if (Collider* collider = player_->GetComponent<Collider>())
	{
		collisionManager_->SetColliderList(collider);
	}

	//プレイヤーの武器を衝突マネージャーに追加
	if (Collider* collider = playerWeapon_->GetComponent<Collider>())
	{
		collisionManager_->SetColliderList(collider);
	}

	//敵を衝突マネージャーに追加
	if (Collider* collider = enemy_->GetComponent<Collider>())
	{
		collisionManager_->SetColliderList(collider);
	}

	//敵の武器を衝突マネージャーに追加
	if (Collider* collider = enemyWeapon_->GetComponent<Collider>())
	{
		collisionManager_->SetColliderList(collider);
	}

	//魔法を衝突マネージャーに追加
	std::vector<MagicProjectile*> magicProjectiles = gameObjectManager_->GetMutableGameObjects<MagicProjectile>("");
	for (MagicProjectile* magicProjectile : magicProjectiles)
	{
		if (Collider* collider = magicProjectile->GetComponent<Collider>())
		{
			collisionManager_->SetColliderList(collider);
		}
	}

	//レーザーを衝突マネージャーに追加
	std::vector<Laser*> lasers = gameObjectManager_->GetMutableGameObjects<Laser>("");
	for (Laser* laser : lasers)
	{
		if (Collider* collider = laser->GetComponent<Collider>())
		{
			collisionManager_->SetColliderList(collider);
		}
	}

	//柱を衝突マネージャーに追加
	std::vector<Pillar*> pillars = gameObjectManager_->GetMutableGameObjects<Pillar>("");
	for (Pillar* pillar : pillars)
	{
		if (Collider* collider = pillar->GetComponent<Collider>())
		{
			collisionManager_->SetColliderList(collider);
		}
	}

	//衝突判定
	collisionManager_->CheckAllCollisions();
}

void GamePlayScene::HandleCameraShake()
{
	//プレイヤーの武器がヒットした場合、カメラシェイクを開始
	if (playerWeapon_->GetIsHit())
	{
		cameraController_->StartCameraShake(playerWeapon_->GetEffectSettings().cameraShakeIntensity, playerWeapon_->GetEffectSettings().cameraShakeDuration);
	}
}

void GamePlayScene::UpdateCameraAndLockOn()
{
	//ロックオンの更新
	lockon_->Update(camera_);

	//カメラパスマネージャーの更新
	cameraPathManager_->Update();

	//カメラコントローラーの更新
	cameraController_->Update();

	//シャドウマップ用のカメラを移動させる
	Camera* shadowCamera = CameraManager::GetCamera("ShadowCamera");
	shadowCamera->translation_ = { camera_->translation_.x, shadowCamera->translation_.y, camera_->translation_.z };

	//カメラの更新
	*camera_ = cameraController_->GetCamera();
	camera_->TransferMatrix();
}

void GamePlayScene::HandleTransition()
{
	//FadeInしていないとき
	if (transition_->GetFadeState() != transition_->FadeState::In)
	{
		//クリアアニメーションが終了していたらゲームクリアのフラグを立てる
		if (cameraController_->GetIsClearAnimationFinished())
		{
			isGameClear_ = true;
			player_->SetIsGameFinished(true);
			enemy_->SetIsGameFinished(true);
		}
		//プレイヤーが死亡状態かつアニメーションが終了していた場合ゲームオーバーのフラグを立てる
		else if (player_->GetIsDead() && player_->GetIsAnimationFinished())
		{
			isGameOver_ = true;
			player_->SetIsGameFinished(true);
			enemy_->SetIsGameFinished(true);
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
	if (transition_->GetIsFadeInComplete())
	{
		audio_->StopAudio(audioHandle_);
		sceneManager_->ChangeScene("GameTitleScene");
	}
}