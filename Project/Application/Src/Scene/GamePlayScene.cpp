/**
 * @file GamePlayScene.cpp
 * @brief ゲームプレイシーンを管理するファイル
 * @author 青木智滉
 * @date
 */

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
	InitializeCameraController();

	//プレイヤーの初期化
	InitializePlayer();

	//敵の初期化
	InitializeEnemy();

	//スプライトの初期化
	InitializeSprites();

	//音声データの読み込みと再生
	LoadAndPlayBGM();
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

void GamePlayScene::InitializeCameraController()
{
	//カメラコントローラーの初期化
	cameraController_ = std::make_unique<CameraController>();
	cameraController_->Initialize();

	//カメラアニメーションエディター、ロックオン、ロックオンターゲットを設定
	cameraController_->SetCameraAnimationEditor(editorManager_->GetCameraAnimationEditor());
	cameraController_->SetLockon(lockon_.get());
}

void GamePlayScene::InitializePlayer()
{
	//プレイヤーの初期化
	player_ = gameObjectManager_->GetGameObject<Player>("Player");

	//キャラクターの初期化
	InitializeCharacter(player_);

	//ロックオンを設定
	player_->SetLockon(lockon_.get());

	//カメラコントローラーにプレイヤーを設定
	cameraController_->SetTarget(player_->GetComponent<TransformComponent>());
}

void GamePlayScene::InitializeEnemy()
{
	//敵の初期化
	enemy_ = gameObjectManager_->GetGameObject<Enemy>("Enemy");

	//キャラクターの初期化
	InitializeCharacter(enemy_);
}

void GamePlayScene::InitializeCharacter(BaseCharacter* character)
{
	//カメラコントローラーを設定
	character->SetCameraController(cameraController_.get());

	//ヒットストップを設定
	character->SetHitStop(hitStop_.get());

	//エディターマネージャーを設定
	character->SetEditorManager(editorManager_.get());

	//コンバットアニメーションエディターにキャラクターを設定
	editorManager_->GetCombatAnimationEditor()->AddEditableCharacter(character);

	//キャラクターをコライダーの配列に追加
	colliders_.push_back(character->GetComponent<Collider>());

	//武器の生成
	InitializeWeapon(character);
}

void GamePlayScene::InitializeWeapon(BaseCharacter* character)
{
	//武器の名前を設定
	std::string weaponName = character->GetName() + "Weapon";

	//プレイヤーの武器を生成
	Weapon* weapon = gameObjectManager_->CreateGameObject<Weapon>(weaponName);

	//キャラクターの右手に親子付け
	TransformComponent* weaponTransform = weapon->GetComponent<TransformComponent>();
	weaponTransform->worldTransform_.SetParent(&character->GetComponent<ModelComponent>()->GetModel()->GetJointWorldTransform("mixamorig:RightHand"));

	//モデルの追加
	ModelComponent* modelComponent = weapon->AddComponent<ModelComponent>();
	modelComponent->SetModel(ModelManager::CreateFromModelFile(weaponName, Opaque));

	//コライダーの追加
	OBBCollider* collider = weapon->AddComponent<OBBCollider>();
	CollisionAttributeManager* collisionAttrManager = CollisionAttributeManager::GetInstance();
	collider->SetCollisionAttribute(collisionAttrManager->GetAttribute(weaponName));
	collider->SetCollisionMask(collisionAttrManager->GetMask(weaponName));

	//キャラクターに武器を設定
	character->SetWeapon(weapon);

	//コライダーの配列に追加
	colliders_.push_back(collider);
}

void GamePlayScene::InitializeSprites()
{
	//ゲームオーバーのスプライトの生成
	TextureManager::Load("GameOver.png");
	gameOverSprite_.reset(Sprite::Create("GameOver.png", { 0.0f,0.0f }));

	//ゲームクリアのスプライトの生成
	TextureManager::Load("GameClear.png");
	gameClearSprite_.reset(Sprite::Create("GameClear.png", { 0.0f,0.0f }));
}

void GamePlayScene::LoadAndPlayBGM()
{
	//音声データの読み込みと再生
	audioHandle_ = audio_->LoadAudioFile("GameScene.mp3");
	voiceHandle_ = audio_->PlayAudio(audioHandle_, true, 0.1f);
}

void GamePlayScene::UpdateColliders()
{
	//コライダーをクリア
	collisionManager_->ClearColliderList();

	//コライダーを追加
	for (Collider* collider : colliders_)
	{
		collisionManager_->SetColliderList(collider);
	}

	//魔法を衝突マネージャーに追加
	for (Magic* magicProjectile : gameObjectManager_->GetGameObjects<Magic>("Magic"))
	{
		collisionManager_->SetColliderList(magicProjectile->GetComponent<Collider>());
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
		audio_->StopAudio(voiceHandle_);
		sceneManager_->ChangeScene("GameTitleScene");
	}

	//FadeInしていないとき
	if (transition_->GetFadeState() != transition_->FadeState::In)
	{
		if (isGameClear_ || isGameOver_) return;

		//敵が死亡状態の場合ゲームクリアのフラグを立てる
		if (enemy_->GetIsGameFinished())
		{
			isGameClear_ = true;
			player_->SetIsGameFinished(true);
		}
		//プレイヤーが死亡状態の場合ゲームオーバーのフラグを立てる
		else if (player_->GetIsGameFinished())
		{
			isGameOver_ = true;
			enemy_->SetIsGameFinished(true);
		}
	}
}