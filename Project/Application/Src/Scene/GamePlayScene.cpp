#include "GamePlayScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Engine/Utilities/RandomGenerator.h"
#include "Engine/LevelLoader/LevelLoader.h"

void GamePlayScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//ゲームオブジェクトをクリア
	gameObjectManager_ = GameObjectManager::GetInstance();
	gameObjectManager_->Clear();


	//パーティクルをクリア
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Clear();

	//
	LevelLoader::Load("GameScene");

	//衝突マネージャーの生成
	collisionManager_ = std::make_unique<CollisionManager>();

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
	player_ = gameObjectManager_->GetGameObject<Player>("Player");
	player_->SetCamera(&camera_);
	player_->SetLockOn(lockOn_.get());
	player_->GetCollider()->SetCollisionAttribute(kCollisionAttributePlayer);
	player_->GetCollider()->SetCollisionMask(kCollisionMaskPlayer);
	player_->GetCollider()->SetCollisionPrimitive(kCollisionPrimitiveAABB);
	//追従対象にプレイヤーを設定
	followCamera_->SetTarget(&player_->GetWorldTransform());

	//ボスの生成
	boss_ = gameObjectManager_->GetGameObject<Boss>("Boss");
	boss_->GetCollider()->SetCollisionAttribute(kCollisionAttributeEnemy);
	boss_->GetCollider()->SetCollisionMask(kCollisionMaskEnemy);
	boss_->GetCollider()->SetCollisionPrimitive(kCollisionPrimitiveAABB);
	bossModel_ = boss_->GetModel();
	bossModel_->GetMaterial(0)->SetEnableLighting(false);
	bossModel_->GetMaterial(0)->SetColor({ 0.9f, 0.5f, 0.9f, 1.0f });

	//地面の生成
	ground_ = gameObjectManager_->GetGameObject<Ground>("Ground");
	groundModel_ = ground_->GetModel();
	groundModel_->GetMaterial(1)->SetEnableLighting(false);

	//トランジションの初期化
	transitionSprite_.reset(Sprite::Create("white.png", { 0.0f,0.0f }));
	transitionSprite_->SetSize({ 1280.0f,720.0f });
	transitionSprite_->SetColor(transitionSpriteColor_);

	//ガイドのスプライトの生成
	TextureManager::Load("Guide.png");
	guideSprite_.reset(Sprite::Create("Guide.png", { 0.0f,0.0f }));

	//BGMの読み込みと再生
	bgmHandle_ = audio_->LoadAudioFile("Application/Resources/Sounds/GamePlay.mp3");
	audio_->PlayAudio(bgmHandle_, true, 0.5f);

	//Skyboxの生成
	TextureManager::Load("Skybox.dds");
	skybox_.reset(Skybox::Create("Skybox.dds"));
	backGround_ = std::make_unique<BackGround>();
	backGround_->Initialize(skybox_.get());

	//ポーズ画面のスプライトの作成
	TextureManager::Load("Pause.png");
	TextureManager::Load("Arrow.png");
	TextureManager::Load("Cursor.png");
	pauseSprite_.reset(Sprite::Create("Pause.png", { 0.0f,0.0f }));
	arrowSprite_.reset(Sprite::Create("Arrow.png", { 0.0f,0.0f }));
	cursorSprite_.reset(Sprite::Create("Cursor.png", cameraSensitivityPositions_[2]));
}

void GamePlayScene::Finalize()
{

}

void GamePlayScene::Update()
{
	//ポーズの更新
	UpdatePause();

	//トランジションの更新
	UpdateTransition();

	if (isCleared_)
	{
		ClearAnimation();

		ImGui::Begin("GamePlayScene");
		ImGui::DragFloat("nFocusWidth", &nFocusWidth_, 0.0001f);
		ImGui::DragFloat("fFocusWidth", &nFocusWidth_, 0.0001f);
		ImGui::DragFloat("focusDepth", &focusDepth_, 0.001f);
		ImGui::End();
	}

	if (!isPause_ && !isCleared_)
	{
		if (input_->IsPushKeyEnter(DIK_C))
		{
			isCleared_ = true;
			followCamera_->SetClearAnimation(true);
			PostEffects::GetInstance()->GetDepthOfField()->SetIsEnable(true);
			PostEffects::GetInstance()->GetDepthOfField()->SetNFocusWidth(nFocusWidth_);
			PostEffects::GetInstance()->GetDepthOfField()->SetFocusDepth(focusDepth_);
			PostEffects::GetInstance()->GetDepthOfField()->SetFFocusWidth(fFocusWidth_);
		}

		//Skyboxの更新
		backGround_->Update();

		//パーティクルの更新
		particleManager_->Update();

		//プレイヤーの攻撃がヒットしたか、またはプレイヤーがダメージを受けたとき
		if (player_->GetWeapon()->GetIsHit() || player_->GetIsHit())
		{
			//カメラシェイクのフラグを立てる
			cameraShakeEnable_ = true;

			//最後の攻撃の時
			if (player_->GetComboIndex() == 3)
			{
				shakeIntensityX = 0.0f;
				shakeIntensityY = 0.4f;
			}
			//プレイヤーがダメージを食らった時
			else if (player_->GetIsHit())
			{
				shakeIntensityX = 0.6f;
				shakeIntensityY = 0.6f;
			}
			//そのほか
			else
			{
				shakeIntensityX = 0.0f;
				shakeIntensityY = 0.1f;
			}
		}

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

		//カメラの更新
		camera_.UpdateMatrix();

		//ヒットストップ中じゃないときに武器が当たったら
		if (!isStop_ && player_->GetWeapon()->GetIsHit())
		{
			//ヒットストップのフラグを立てる
			isStop_ = true;
			//最後の攻撃の時はヒットストップを長めに
			stopTime_ = (player_->GetComboIndex() == 3) ? 10 : 2;
			//ヒットフラグをリセット
			player_->GetWeapon()->SetIsHit(false);
		}

		//ヒットストップの処理
		if (isStop_)
		{
			if (++stopTimer_ >= stopTime_)
			{
				isStop_ = false;
				stopTimer_ = 0;
			}
			return;
		}

		//プレイヤーがジャスト回避をしていたら
		if (player_->GetisJustAvoid())
		{
			boss_->SetIsSlow(true);
			PostEffects::GetInstance()->GetGrayScale()->SetIsEnable(true);
			//PostEffects::GetInstance()->GetGrayScale()->SetIsSepiaEnabled(true);
			//PostEffects::GetInstance()->GetVignette()->SetIsEnable(true);
		}

		if (boss_->GetIsSlow())
		{
			if (length <= 1.0f)
			{
				length += 0.1f;
			}
			//PostEffects::GetInstance()->GetVignette()->SetIntensity(length);
		}
		else
		{
			length = 0.0f;
			PostEffects::GetInstance()->GetGrayScale()->SetIsEnable(false);
			//PostEffects::GetInstance()->GetGrayScale()->SetIsSepiaEnabled(false);
			//PostEffects::GetInstance()->GetVignette()->SetIsEnable(false);
			//PostEffects::GetInstance()->GetVignette()->SetIntensity(0.0f);
			player_->SetJustAvoidInvincible(false);
		}

		//ゲームオブジェクトの更新
		gameObjectManager_->Update();

		//ロックオンの処理
		lockOn_->Update(boss_, camera_);

		//追従カメラの更新
		followCamera_->Update();
		camera_ = followCamera_->GetCamera();

		//衝突判定
		collisionManager_->ClearColliderList();
		if (player_->GetCollider())
		{
			collisionManager_->SetColliderList(player_->GetCollider());
		}
		Weapon* weapon = player_->GetWeapon();
		if (weapon->GetIsAttack())
		{
			if (weapon->GetCollider())
			{
				collisionManager_->SetColliderList(weapon->GetCollider());
			}
		}
		for (const std::unique_ptr<Missile>& missile : boss_->GetMissiles())
		{
			if (missile->GetCollider())
			{
				collisionManager_->SetColliderList(missile->GetCollider());
			}
		}
		for (const std::unique_ptr<Laser>& laser : boss_->GetLasers())
		{
			if (laser->GetCollider())
			{
				collisionManager_->SetColliderList(laser->GetCollider());
			}
		}
		if (boss_->GetCollider())
		{
			collisionManager_->SetColliderList(boss_->GetCollider());
		}
		collisionManager_->CheckAllCollisions();
	}

	ImGui::Begin("GamePlayScene");
	ImGui::DragFloat2("CursorPosition", &cursorSpritePosition_.x);
	ImGui::End();
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

#pragma region Skyboxの描画
	//Skybox描画前処理
	renderer_->PreDrawSkybox();

	//Skyboxの描画
	backGround_->Draw(camera_);

	//Skybox描画処理
	renderer_->PostDrawSkybox();
#pragma endregion

#pragma region 3Dオブジェクト描画
	//ゲームオブジェクトのモデル描画
	gameObjectManager_->Draw(camera_);

	//3Dオブジェクト描画
	renderer_->Render();
#pragma endregion

#pragma region パーティクル描画
	//パーティクル描画前処理
	renderer_->PreDrawParticles();

	//パーティクルの描画
	particleManager_->Draw(camera_);

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

	//ガイドのスプライトの描画
	guideSprite_->Draw();

	//ポーズ画面
	if (isPause_)
	{
		pauseSprite_->Draw();
		arrowSprite_->Draw();
		cursorSprite_->Draw();
	}

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
			PostEffects::GetInstance()->GetDepthOfField()->SetIsEnable(false);
		}
	}

	//トランジションが行われていないときに入力を受け付ける
	if (!isFadeOut_ && !isFadeIn_)
	{
		//ボスの体力が0になったらゲームクリア
		if (boss_->GetHP() <= 0.0f)
		{
			//isFadeIn_ = true;
			nextScene_ = kGameClear;
			PostEffects::GetInstance()->GetDepthOfField()->SetIsEnable(true);
			PostEffects::GetInstance()->GetDepthOfField()->SetNFocusWidth(nFocusWidth_);
			PostEffects::GetInstance()->GetDepthOfField()->SetFocusDepth(focusDepth_);
			PostEffects::GetInstance()->GetDepthOfField()->SetFFocusWidth(fFocusWidth_);
			followCamera_->SetClearAnimation(true);
			isCleared_ = true;
		}

		//プレイヤーの体力が0になったらゲームオーバー
		if (player_->GetHP() <= 0.0f)
		{
			isFadeIn_ = true;
			nextScene_ = kGameOver;
		}

		//キーボード
		if (input_->IsPushKeyEnter(DIK_T))
		{
			isFadeIn_ = true;
		}
	}
}

void GamePlayScene::UpdatePause()
{
	if (!isFadeIn_ && !isFadeOut_)
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_START))
		{
			if (isPause_)
			{
				isPause_ = false;
				PostEffects::GetInstance()->GetBoxFilter()->SetIsEnable(false);
				PostEffects::GetInstance()->GetBoxFilter()->SetKernelSize(KernelSize::k5x5);
			}
			else
			{
				isPause_ = true;
				PostEffects::GetInstance()->GetBoxFilter()->SetIsEnable(true);
				PostEffects::GetInstance()->GetBoxFilter()->SetKernelSize(KernelSize::k5x5);
			}
		}
	}

	if (isPause_)
	{
		//スティックの操作
		const float threshold = 0.7f;
		float axis = input_->GetLeftStickX();
		if (std::abs(axis) > threshold)
		{
			if (!isArrowMoved_)
			{
				isArrowMoved_ = true;
				int currentCameraSensitivity = followCamera_->GetCameraSensitivity();
				//右入力
				if (axis > 0.7f)
				{
					currentCameraSensitivity++;
				}
				//左入力
				else
				{
					currentCameraSensitivity--;
				}

				//設定の上限を超えていなかったら
				if (currentCameraSensitivity >= 0 && currentCameraSensitivity < 5)
				{
					followCamera_->SetCameraSensitivity(currentCameraSensitivity);
					cursorSprite_->SetPosition(cameraSensitivityPositions_[currentCameraSensitivity]);
				}
			}
		}
		else
		{
			isArrowMoved_ = false;
		}
	}

	arrowSprite_->SetPosition(arrowSpritePosition_);
}

void GamePlayScene::ClearAnimation()
{
	if (followCamera_->GetClearAnimationEnd())
	{
		isFadeIn_ = true;
	}

	followCamera_->Update();
	camera_ = followCamera_->GetCamera();
	camera_.UpdateMatrix();
}