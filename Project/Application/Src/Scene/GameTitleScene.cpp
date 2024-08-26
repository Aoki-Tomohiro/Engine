#include "GameTitleScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Components/Animator/AnimatorComponent.h"

void GameTitleScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//ゲームオブジェクトマネージャーの初期化
	gameObjectManager_ = GameObjectManager::GetInstance();
	gameObjectManager_->Clear();

	//パーティクルマネージャーを初期化
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Clear();

	//レベルデータの読み込み
	LevelLoader::Load("TitleScene");

	//カメラを取得
	camera_ = CameraManager::GetInstance()->GetCamera("Camera");
	camera_->rotationType_ = RotationType::Euler;
	//ゲームオブジェクトマネージャーにカメラを設定
	gameObjectManager_->SetCamera(camera_);
	//パーティクルマネージャーにカメラを設定
	particleManager_->SetCamera(camera_);

	//プレイヤーを取得
	Player* player = gameObjectManager_->GetMutableGameObject<Player>("");
	//アニメーターコンポーネントを追加
	AnimatorComponent* animatorComponent = player->AddComponent<AnimatorComponent>();
	animatorComponent->AddAnimation("Idle", AnimationManager::Create("Player/Animations/Idle.gltf"));
	animatorComponent->PlayAnimation("Idle", 1.0f, true);

	//トランジションの生成
	transition_ = std::make_unique<Transition>();
	transition_->Initialize();
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

	//入力に基づいてフェードインをトリガーしフェードインが完了したらシーンを遷移させる
	TriggerFadeInAndChangeScene();
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
	Vector3 offset = { 0.0f,10.0f ,-20.0f };
	offset = Mathf::TransformNormal(offset, rotateYMatrix);

	//カメラの座標を設定
	camera_->translation_ = offset;

	//カメラの行列の更新
	camera_->UpdateMatrix();
}

void GameTitleScene::TriggerFadeInAndChangeScene()
{
	//Aボタンかスペースキーを押したらフェードインさせる
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
	{
		transition_->SetFadeState(Transition::FadeState::In);
	}
	else if (input_->IsPushKeyEnter(DIK_SPACE))
	{
		transition_->SetFadeState(Transition::FadeState::In);
	}

	//シーン遷移
	if (transition_->GetIsFadeInComplete())
	{
		sceneManager_->ChangeScene("GamePlayScene");
	}
}