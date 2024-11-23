#include "GamePlayScene.h"
#include "Engine/Framework/Scene/SceneManager.h"

void GamePlayScene::Initialize()
{
	//レンダラーのインスタンスを取得
	renderer_ = Renderer::GetInstance();

	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//オーディオのインスタンスを取得
	audio_ = Audio::GetInstance();

	//ゲームオブジェクトマネージャーの初期化
	gameObjectManager_ = GameObjectManager::GetInstance();
	gameObjectManager_->Clear();

	//レベルデータの読み込み
	LevelManager::LoadLevelAndCreateObjects("GameScene");

	//カメラの初期化
	camera_ = CameraManager::GetInstance()->GetCamera("Camera");
	camera_->rotationType_ = RotationType::Euler;

	//TrailRendererにカメラを設定
	TrailRenderer::GetInstance()->SetCamera(camera_);

	//衝突マネージャーの生成
	collisionManager_ = std::make_unique<CollisionManager>();

	//ヒットストップの生成
	hitStop_ = std::make_unique<HitStop>();

	//ロックオンの初期化
	lockon_ = std::make_unique<Lockon>();
	lockon_->Initialize();

	//トランジションの生成
	transition_ = std::make_unique<Transition>();
	transition_->Initialize();

	//エディターマネージャーの作成
	editorManager_ = std::make_unique<EditorManager>();
	editorManager_->Initialize();

	//カメラコントローラーの初期化
	cameraController_ = std::make_unique<CameraController>();
	cameraController_->Initialize();
	//カメラアニメーションエディター、ロックオン、ロックオンターゲットを設定
	cameraController_->SetCameraAnimationEditor(editorManager_->GetCameraAnimationEditor());
	cameraController_->SetLockon(lockon_.get());

	//プレイヤーの初期化
	player_ = gameObjectManager_->GetGameObject<Player>("Player");
	//カメラコントローラー、ロックオン、ヒットストップ、エディターマネージャーを設定
	player_->SetCameraController(cameraController_.get());
	player_->SetLockon(lockon_.get());
	player_->SetHitStop(hitStop_.get());
	player_->SetEditorManager(editorManager_.get());
	//コンバットアニメーションエディターにプレイヤーを設定
	editorManager_->GetCombatAnimationEditor()->AddEditableCharacter(player_);
	//カメラコントローラーに追従対象を設定
	cameraController_->SetTarget(player_->GetComponent<TransformComponent>());

	//敵の初期化
	enemy_ = gameObjectManager_->GetGameObject<Enemy>("Enemy");
	//カメラコントローラー、エディターマネージャー、ヒットストップを設定
	enemy_->SetCameraController(cameraController_.get());
	enemy_->SetEditorManager(editorManager_.get());
	enemy_->SetHitStop(hitStop_.get());
	//コンバットアニメーションエディターに敵を設定
	editorManager_->GetCombatAnimationEditor()->AddEditableCharacter(enemy_);

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
	//エディターマネージャーの更新
	editorManager_->Update();

	//ゲームオブジェクトマネージャーの更新
	gameObjectManager_->Update();

	//コライダーをクリアし、必要なオブジェクトを追加
	UpdateColliders();

	//カメラとロックオンの更新
	UpdateCameraAndLockOn();

	//ヒットストップの更新
	hitStop_->Update();

	//トランジションの更新
	transition_->Update();
	HandleTransition();
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
	gameObjectManager_->Draw(*camera_);

	//3Dオブジェクト描画
	renderer_->Render();
#pragma endregion

#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void GamePlayScene::DrawUI()
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//ゲームオブジェクトのUIを描画
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
	if (Collider* collider = gameObjectManager_->GetGameObject<Weapon>("PlayerWeapon")->GetComponent<Collider>())
	{
		collisionManager_->SetColliderList(collider);
	}

	//敵を衝突マネージャーに追加
	if (Collider* collider = enemy_->GetComponent<Collider>())
	{
		collisionManager_->SetColliderList(collider);
	}

	//敵の武器を衝突マネージャーに追加
	if (Collider* collider = gameObjectManager_->GetGameObject<Weapon>("EnemyWeapon")->GetComponent<Collider>())
	{
		collisionManager_->SetColliderList(collider);
	}

	//魔法を衝突マネージャーに追加
	std::vector<Magic*> magicProjectiles = gameObjectManager_->GetGameObjects<Magic>("Magic");
	for (Magic* magicProjectile : magicProjectiles)
	{
		if (Collider* collider = magicProjectile->GetComponent<Collider>())
		{
			collisionManager_->SetColliderList(collider);
		}
	}

	//レーザーを衝突マネージャーに追加
	std::vector<Laser*> lasers = gameObjectManager_->GetGameObjects<Laser>("Laser");
	for (Laser* laser : lasers)
	{
		if (Collider* collider = laser->GetComponent<Collider>())
		{
			collisionManager_->SetColliderList(collider);
		}
	}

	//柱を衝突マネージャーに追加
	std::vector<Pillar*> pillars = gameObjectManager_->GetGameObjects<Pillar>("Pillar");
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

void GamePlayScene::UpdateCameraAndLockOn()
{
	//ロックオンの更新
	lockon_->Update(camera_);

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
		//敵が死亡状態の場合ゲームクリアのフラグを立てる
		if (enemy_->GetIsGameFinished())
		{
			isGameClear_ = true;
		}
		//プレイヤーが死亡状態の場合ゲームオーバーのフラグを立てる
		else if (player_->GetIsGameFinished())
		{
			isGameOver_ = true;
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