#include "GameTitleScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include <numbers>

void GameTitleScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//GameObjectManagerの初期化
	gameObjectManager_ = GameObjectManager::GetInstance();
	gameObjectManager_->Clear();

	//ParticleManagerを初期化
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Clear();

	//LevelDataの読み込み
	LevelLoader::Load("TitleScene");

	//カメラを取得
	camera_ = CameraManager::GetInstance()->GetCamera("Camera");
	camera_->rotationType_ = RotationType::Euler;
	//GameObjectManagerにカメラを設定
	gameObjectManager_->SetCamera(camera_);
	//ParticleManagerにカメラを設定
	particleManager_->SetCamera(camera_);

	//プレイヤーの初期化
	Player* player = gameObjectManager_->GetGameObject<Player>();
	player->SetIsInTitleScene(true);
	//ModelComponent
	ModelComponent* playerModelComponent = player->GetComponent<ModelComponent>();
	playerModelComponent->SetAnimationName("Armature|mixamo.com|Layer0");
	playerModelComponent->GetModel()->GetAnimation()->SetIsLoop(true);
	playerModelComponent->GetModel()->GetMaterial(0)->SetDissolveThreshold(0.0f);
	playerModelComponent->GetModel()->GetMaterial(1)->SetDissolveThreshold(0.0f);
	playerModelComponent->GetModel()->GetMaterial(2)->SetDissolveThreshold(0.0f);
	playerModelComponent->GetModel()->GetMaterial(3)->SetDissolveThreshold(0.0f);
	playerModelComponent->GetModel()->GetMaterial(4)->SetDissolveThreshold(0.0f);
	playerModelComponent->GetModel()->GetMaterial(5)->SetDissolveThreshold(0.0f);
	playerModelComponent->GetModel()->GetMaterial(6)->SetDissolveThreshold(0.0f);

	//敵の初期化
	Enemy* enemy = gameObjectManager_->GetGameObject<Enemy>();
	enemy->SetIsInTitleScene(true);
	//ModelComponentの初期化
	ModelComponent* enemyModelComponent = enemy->GetComponent<ModelComponent>();
	enemyModelComponent->SetAnimationName("Armature|mixamo.com|Layer0");
	enemyModelComponent->GetModel()->GetAnimation()->SetIsLoop(true);
	enemyModelComponent->GetModel()->GetMaterial(0)->SetDissolveThreshold(0.0f);
	enemyModelComponent->GetModel()->GetMaterial(1)->SetDissolveThreshold(0.0f);

	//トランジションの生成
	transition_ = std::make_unique<Transition>();
	transition_->Initialize();

	//Skyboxの初期化
	skybox_.reset(Skybox::Create());

	//タイトルのスプライトの生成
	TextureManager::Load("GameTitle.png");
	titleSprite_.reset(Sprite::Create("GameTitle.png", { 0.0f,0.0f }));
	TextureManager::Load("PressA.png");
	pressASprite_.reset(Sprite::Create("PressA.png", { 0.0f,0.0f }));

	//音声データの読み込みと再生
	audioHandle_ = audio_->LoadAudioFile("TitleScene.mp3");
	audio_->PlayAudio(audioHandle_, true, 0.2f);
}

void GameTitleScene::Finalize()
{

}

void GameTitleScene::Update()
{
	//GameObjectManagerの更新
	gameObjectManager_->Update();

	//トランジションの更新
	transition_->Update();

	//カメラの更新
	CameraUpdate();

	//カメラの行列の更新
	camera_->UpdateMatrix();

	//フェードイン処理
	HandleTransition();

	//ImGui
	ImGui::Begin("GameTitleScene");
	ImGui::Text("Space or AButton: GamePlayScene");
	ImGui::DragFloat3("CameraTranslation", &camera_->translation_.x, 0.1f);
	ImGui::DragFloat3("CameraRotation", &camera_->rotation_.x, 0.01f);
	ImGui::End();
}

void GameTitleScene::Draw()
{
#pragma region Skybox描画
	//Skybox描画前処理
	renderer_->PreDrawSkybox();

	//Skyboxの描画
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
	//GameObjectManagerの描画
	gameObjectManager_->Draw();

	//3Dオブジェクト描画
	renderer_->Render();
#pragma endregion
}

void GameTitleScene::DrawUI()
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//タイトルのスプライトの描画
	titleSprite_->Draw();

	//PressAのスプライトの描画
	pressASprite_->Draw();

	//トランジションの描画
	transition_->Draw();

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void GameTitleScene::CameraUpdate()
{
	//回転速度
	const float kRotSpeed = 0.006f;

	//回転処理
	camera_->rotation_.y += kRotSpeed;
	camera_->rotation_.y = std::fmod(camera_->rotation_.y, std::numbers::pi_v<float> * 2.0f);

	//回転行列の作成
	Matrix4x4 rotateYMatrix = Mathf::MakeRotateYMatrix(camera_->rotation_.y);

	//Offsetに回転行列を適用
	Vector3 offset = { 0.0f,30.0f ,-80.0f };
	offset = Mathf::TransformNormal(offset, rotateYMatrix);

	//カメラの座標を設定
	camera_->translation_ = offset;
}

void GameTitleScene::HandleTransition()
{
	//SpaceキーかAボタンを押したらフェードインさせる
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
	{
		transition_->SetFadeState(Transition::FadeState::In);
	}
	else if (input_->IsPushKeyEnter(DIK_SPACE))
	{
		transition_->SetFadeState(Transition::FadeState::In);
	}

	//シーン遷移
	if (transition_->GetFadeInComplete())
	{
		audio_->StopAudio(audioHandle_);
		sceneManager_->ChangeScene("GamePlayScene");
	}
}