#include "GamePlayScene.h"
#include "Engine/Framework/Scene/SceneManager.h"

void GamePlayScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//ゲームオブジェクトをクリア
	gameObjectManager_ = GameObjectManager::GetInstance();
	gameObjectManager_->Clear();

	//カメラの初期化
	camera_.Initialize();

	//ロックオンの初期化
	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();

	//追従カメラの生成
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	followCamera_->SetLockOn(lockOn_.get());

	//プレイヤーの生成
	playerModel_.reset(ModelManager::CreateFromOBJ("Player", Opaque));
	playerModel_->SetEnableLighting(false);
	playerModel_->SetColor({ 0.45f, 0.85f, 0.45f, 1.0f });
	player_ = GameObjectManager::CreateGameObject<Player>();
	player_->SetModel(playerModel_.get());
	player_->SetTag("Player");
	player_->SetCamera(&camera_);
	player_->SetLockOn(lockOn_.get());
	//追従対象にプレイヤーを設定
	followCamera_->SetTarget(&player_->GetWorldTransform());

	//ボスの生成
	bossModel_.reset(ModelManager::CreateFromOBJ("Boss", Opaque));
	bossModel_->SetEnableLighting(false);
	bossModel_->SetColor({ 0.9f, 0.5f, 0.9f, 1.0f });
	boss_ = GameObjectManager::CreateGameObject<Boss>();
	boss_->SetModel(bossModel_.get());
	boss_->SetTag("Boss");

	//天球の作成
	skydomeModel_.reset(ModelManager::CreateFromOBJ("Skydome", Opaque));
	skydomeModel_->SetEnableLighting(false);
	skydome_ = GameObjectManager::CreateGameObject<Skydome>();
	skydome_->SetModel(skydomeModel_.get());

	//地面の生成
	groundModel_.reset(ModelManager::CreateFromOBJ("Cube", Opaque));
	groundModel_->SetEnableLighting(false);
	groundModel_->SetColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	ground_ = GameObjectManager::CreateGameObject<Ground>();
	ground_->SetModel(groundModel_.get());

	//トランジションの初期化
	transitionSprite_.reset(Sprite::Create("white.png", { 0.0f,0.0f }));
	transitionSprite_->SetSize({ 1280.0f,720.0f });
	transitionSprite_->SetColor(transitionSpriteColor_);

	//BGMの読み込みと再生
	bgmHandle_ = audio_->SoundLoadWave("Project/Resources/Sounds/GamePlay.wav");
	audio_->SoundPlayWave(bgmHandle_, true, 0.5f);
}

void GamePlayScene::Finalize()
{

}

void GamePlayScene::Update()
{
	//トランジションの更新
	UpdateTransition();

	//ゲームオブジェクトの更新
	gameObjectManager_->Update();

	//ロックオンの処理
	lockOn_->Update(boss_, camera_);

	//追従カメラの更新
	followCamera_->Update();

	//カメラの更新
	camera_ = followCamera_->GetCamera();
	camera_.UpdateMatrix();
}

void GamePlayScene::Draw() 
{
#pragma region 背景スプライト描画
	//背景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//背景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion

	//深度バッファをクリア
	renderer_->ClearDepthBuffer();

#pragma region 3Dオブジェクト描画
	//ゲームオブジェクトのモデル描画
	gameObjectManager_->Draw(camera_);

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

void GamePlayScene::DrawUI()
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//ゲームオブジェクトのスプライト描画
	gameObjectManager_->DrawUI();

	//ロックオンの描画
	lockOn_->Draw();

	//トランジション用のスプライトの描画
	transitionSprite_->Draw();

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void GamePlayScene::UpdateTransition()
{
	//フェードアウトの処理
	if (isFadeOut_)
	{
		//徐々に透明にする
		transitionTimer_ += 1.0f / 60.0f;
		transitionSpriteColor_.w = Mathf::Lerp(transitionSpriteColor_.w, 0.0f, transitionTimer_);
		transitionSprite_->SetColor(transitionSpriteColor_);

		//完全に透明になったら終了
		if (transitionSpriteColor_.w <= 0.0f)
		{
			isFadeOut_ = false;
			transitionTimer_ = 0.0f;
		}
	}

	//フェードインの処理
	if (isFadeIn_)
	{
		//徐々に暗くする
		transitionTimer_ += 1.0f / 60.0f;
		transitionSpriteColor_.w = Mathf::Lerp(transitionSpriteColor_.w, 1.0f, transitionTimer_);
		transitionSprite_->SetColor(transitionSpriteColor_);

		//完全に暗くなったらシーンを変える
		if (transitionSpriteColor_.w >= 1.0f)
		{
			switch (nextScene_)
			{
			case kGameClear:
				sceneManager_->ChangeScene("GameClearScene");
				break;
			case kGameOver:
				sceneManager_->ChangeScene("GameOverScene");
				break;
			}
			audio_->StopAudio(bgmHandle_);
		}
	}

	//トランジションが行われていないときに入力を受け付ける
	if (!isFadeOut_ && !isFadeIn_)
	{
		//コントローラー
		if (input_->IsControllerConnected())
		{
			if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
			{
				isFadeIn_ = true;
			}
		}

		//キーボード
		if (input_->IsPushKeyEnter(DIK_SPACE))
		{
			isFadeIn_ = true;
		}
	}
}