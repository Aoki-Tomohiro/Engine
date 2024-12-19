/**
 * @file GameTitleScene.cpp
 * @brief ゲームタイトルシーンを管理するファイル
 * @author 青木智滉
 * @date
 */

#include "GameTitleScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Components/Animator/AnimatorComponent.h"

void GameTitleScene::Initialize()
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
	LevelManager::LoadLevelAndCreateObjects("TitleScene");

	//カメラの初期化
	camera_ = CameraManager::GetInstance()->GetCamera("Camera");
	camera_->rotationType_ = RotationType::Euler;

	//パーティクルマネージャーを初期化
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Clear();
	particleManager_->SetCamera(camera_);

	//TrailRendererにカメラを設定
	TrailRenderer::GetInstance()->SetCamera(camera_);

	//プレイヤーを取得
	Player* player = gameObjectManager_->GetGameObject<Player>("Player");
	//タイトルシーンのフラグを設定
	player->SetIsInTitleScene(true);

	//敵を取得
	enemy_ = gameObjectManager_->GetGameObject<Enemy>("Enemy");
	//タイトルシーンのフラグを設定
	enemy_->SetIsInTitleScene(true);
	//難易度の初期化
	difficultyLevel_ = enemy_->GetLevel();

	//トランジションの生成
	transition_ = std::make_unique<Transition>();
	transition_->Initialize();

	//ゲームタイトルのスプライトの生成
	TextureManager::Load("GameTitle.png");
	titleSprite_.reset(Sprite::Create("GameTitle.png", { 0.0f,0.0f }));

	//PressAのスプライトの生成
	TextureManager::Load("PressA.png");
	pressASprite_.reset(Sprite::Create("PressA.png", { 0.0f,0.0f }));

	//難易度メニューのスプライトを生成
	difficultyMenuSprite_.reset(Sprite::Create("white.png", { 0.0f,0.0f }));
	difficultyMenuSprite_->SetColor(difficultyMenuSpriteColor_);
	difficultyMenuSprite_->SetTextureSize({ static_cast<float>(Application::kClientWidth), static_cast<float>(Application::kClientHeight) });

	//中心のアンカーポイント
	const Vector2 centerAnchorPoint = { 0.5f,0.5f };

	//難易度メニューのテキストのスプライト生成
	TextureManager::Load("DifficultyMenuText.png");
	difficultyMenuTextSprite_.reset(Sprite::Create("DifficultyMenuText.png", difficultyMenuTextPosition_));
	difficultyMenuTextSprite_->SetAnchorPoint(centerAnchorPoint);

	//難易度のテクスチャの名前の配列
	const std::vector<std::string> levelTextures = { "Easy.png","Normal.png","Hard.png" };
	//難易度のスプライトの生成
	for (int32_t i = 0; i < Enemy::Level::Count; ++i)
	{
		TextureManager::Load(levelTextures[i]);
		levelSprites_[i].reset(Sprite::Create(levelTextures[i], levelSpritePosition_[i]));
		levelSprites_[i]->SetAnchorPoint(centerAnchorPoint);
	}

	//音声データの読み込みと再生
	audioHandle_ = audio_->LoadAudioFile("TitleScene.mp3");
	voiceHandle_ = audio_->PlayAudio(audioHandle_, true, 0.1f);
}

void GameTitleScene::Finalize()
{

}

void GameTitleScene::Update()
{
	//ゲームオブジェクトマネージャーの更新
	gameObjectManager_->Update();

	//トランジションの更新
	transition_->Update();

	//カメラの更新
	UpdateCamera();

	//難易度メニューが表示されている場合
	if (isDifficultyMenuVisible_)
	{
		//難易度UIの更新
		UpdateDifficultyUI();

		//フェードインとシーン遷移処理
		HandleFadeAndSceneTransition(); 
	}

	//ボタンが押されたら難易度メニューを表示
	if (!isDifficultyMenuVisible_ && IsActionButtonPressed())
	{
		isDifficultyMenuVisible_ = true;
	}
}

void GameTitleScene::Draw()
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

void GameTitleScene::DrawUI()
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//難易度のメニューが表示されている場合
	if (isDifficultyMenuVisible_)
	{
		//難易度調整のメニュースプライトを描画
		difficultyMenuSprite_->Draw();

		//難易度調整のテキストのスプライトを描画
		difficultyMenuTextSprite_->Draw();

		//難易度のスプライトの描画
		for (int32_t i = 0; i < Enemy::Level::Count; ++i)
		{
			levelSprites_[i]->Draw();
		}
	}
	else
	{
		//タイトルのスプライトの描画
		titleSprite_->Draw();
		//PressAのスプライトの描画
		pressASprite_->Draw();
	}

	//トランジションの描画
	transition_->Draw();

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void GameTitleScene::UpdateCamera()
{
	//回転速度
	const float kRotSpeed = 0.006f;

	//回転処理
	camera_->rotation_.y += kRotSpeed;
	camera_->rotation_.y = std::fmod(camera_->rotation_.y, std::numbers::pi_v<float> *2.0f);

	//回転行列の作成
	Matrix4x4 rotateYMatrix = Mathf::MakeRotateYMatrix(camera_->rotation_.y);

	//Offsetに回転行列を適用
	Vector3 offset = { 0.0f, 20.0f ,-50.0f };
	offset = Mathf::TransformNormal(offset, rotateYMatrix);

	//カメラの座標を設定
	camera_->translation_ = offset;

	//カメラの行列の更新
	camera_->UpdateMatrix();
}

void GameTitleScene::UpdateDifficultyUI()
{
	//スティックの入力の閾値
	const float kThreshold = 0.7f;

	//切り替え時間
	const float kDifficultyChangeDuration = 0.2f;

	//難易度切り替えのタイマーを進める
	difficultyChangeTimer_ += GameTimer::GetDeltaTime();

	//スティックの入力値を取得
	Vector3 inputValue = {
		input_->GetLeftStickX(),
		input_->GetLeftStickY(),
		0.0f
	};

	//スティックの入力が閾値を超えていてかつ切り替えの時間が一定値を超えていた場合
	if (Mathf::Length(inputValue) > kThreshold && difficultyChangeTimer_ > kDifficultyChangeDuration)
	{
		//スティックが上方向に入力されていて現在のレベルが0を下回っていない場合
		if (inputValue.y > 0 && difficultyLevel_ > 0)
		{
			difficultyLevel_ = static_cast<Enemy::Level>(static_cast<int>(difficultyLevel_) - 1);
		}
		//スティックが下方向に入力されていて現在のレベルが最大値を超えていない場合
		else if (inputValue.y < 0 && difficultyLevel_ < Enemy::Level::Count - 1)
		{
			difficultyLevel_ = static_cast<Enemy::Level>(static_cast<int>(difficultyLevel_) + 1);
		}

		//タイマーをリセット
		difficultyChangeTimer_ = 0.0f;
	}

	//UIの色を変える
	for (int32_t i = 0; i < Enemy::Level::Count; ++i)
	{
		//基本の色
		const Vector4 defaultColor = { 1.0f,1.0f,1.0f,1.0f };

		//選択中のレベルの色
		const Vector4 selectedColor = { 0.814f,0.794f,0.148f,1.0f };

		//選択中のレベルだった場合は色を変える
		if (difficultyLevel_ == i)
		{
			levelSprites_[i]->SetColor(selectedColor);
			continue;
		}

		//レベルのスプライトの色を変える
		levelSprites_[i]->SetColor(defaultColor);
	}
}

void GameTitleScene::HandleFadeAndSceneTransition()
{
	//Aボタンかスペースキーを押したらフェードインさせる
	if (IsActionButtonPressed())
	{
		transition_->SetFadeState(Transition::FadeState::In);
	}

	//シーン遷移
	if (transition_->GetIsFadeInComplete())
	{
		enemy_->SetLevel(difficultyLevel_);
		audio_->StopAudio(voiceHandle_);
		sceneManager_->ChangeScene("GamePlayScene");
	}
}

bool GameTitleScene::IsActionButtonPressed()
{
	return input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) || input_->IsPushKeyEnter(DIK_SPACE);
}