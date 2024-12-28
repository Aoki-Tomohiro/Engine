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

	//UIマネージャーの初期化
	uiManager_ = std::make_unique<UIManager>();
	uiManager_->Initialize("GameTitleSceneUI.csv");

	//プレイヤーを取得
	Player* player = gameObjectManager_->GetGameObject<Player>("Player");
	//タイトルシーンのフラグを設定
	player->SetIsInTitleScene(true);

	//敵を取得
	enemy_ = gameObjectManager_->GetGameObject<Enemy>("Enemy");
	//タイトルシーンのフラグを設定
	enemy_->SetIsInTitleScene(true);

	//トランジションの生成
	transition_ = std::make_unique<Transition>();
	transition_->Initialize();

	//レベルセレクターの初期化
	levelSelector_ = std::make_unique<LevelSelector>();
	levelSelector_->Initialize(enemy_, uiManager_.get());

	//音声データの読み込みと再生
	audioHandle_ = audio_->LoadAudioFile("TitleScene.mp3");
	voiceHandle_ = audio_->PlayAudio(audioHandle_, true, 0.1f);

	//ゲームタイトルのUIを取得
	titleUI_ = uiManager_->GetUI<UIElement>("GameTitle");
}

void GameTitleScene::Update()
{
	//UIマネージャーの更新
	uiManager_->Update();

	//ゲームオブジェクトマネージャーの更新
	gameObjectManager_->Update();

	//トランジションの更新
	transition_->Update();

	//カメラの更新
	UpdateCamera();

	//難易度選択メニューが表示中の場合
	if (levelSelector_->GetIsDifficultyMenuVisible())
	{
		//難易度選択メニューの処理
		HandleDifficultyMenu();
	}
	else
	{
		//タイトルメニューの処理
		HandleTitleMenu();
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

	//UIマネージャーの描画
	uiManager_->Draw();

	//トランジションの描画
	transition_->Draw();

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void GameTitleScene::UpdateCamera()
{
	//回転速度
	static const float kRotSpeed = 0.006f;

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

void GameTitleScene::HandleDifficultyMenu()
{
	//トランジションがフェードイン状態でない場合
	if (transition_->GetFadeState() != Transition::FadeState::In)
	{
		//キャンセルボタンが押された場合メニューを閉じてタイトル画面を表示
		if (IsCancelButtonPressed())
		{
			titleUI_->SetIsVisible(true);
			levelSelector_->SetDifficultyMenuVisibility(false);
		}

		//難易度選択メニューの更新
		levelSelector_->Update();
	}

	//フェードインおよびシーン遷移処理
	HandleFadeAndSceneTransition();
}

void GameTitleScene::HandleTitleMenu()
{
	//決定ボタンが押された場合難易度選択メニューを表示
	if (IsActionButtonPressed())
	{
		titleUI_->SetIsVisible(false);
		levelSelector_->SetDifficultyMenuVisibility(true);
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
		audio_->StopAudio(voiceHandle_);
		sceneManager_->ChangeScene("GamePlayScene");
	}
}

bool GameTitleScene::IsActionButtonPressed()
{
	return input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) || input_->IsPushKeyEnter(DIK_SPACE);
}

bool GameTitleScene::IsCancelButtonPressed()
{
	return input_->IsPressButtonEnter(XINPUT_GAMEPAD_B) || input_->IsPushKeyEnter(DIK_ESCAPE);
}