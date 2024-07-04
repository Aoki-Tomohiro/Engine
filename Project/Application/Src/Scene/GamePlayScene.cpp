#include "GamePlayScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/LevelLoader/LevelLoader.h"

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

	//FollowCameraの作成
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	followCamera_->SetTarget(&gameObjectManager_->GetGameObject<Player>()->GetComponent<TransformComponent>()->worldTransform_);

	//LockOnの生成
	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();
	followCamera_->SetLockOn(lockOn_.get());

	//プレイヤーの初期化
	Player* player = gameObjectManager_->GetGameObject<Player>();
	//TransformComponentの初期化
	TransformComponent* playerTransformComponent = player->GetComponent<TransformComponent>();
	playerTransformComponent->worldTransform_.rotationType_ = RotationType::Quaternion;
	//ModelComponent
	ModelComponent* playerModelComponent = player->GetComponent<ModelComponent>();
	//カメラを設定
	player->SetCamera(&camera_);
	//LockOnを設定
	player->SetLockOn(lockOn_.get());

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
	weapon->AddComponent<OBBCollider>();
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
}

void GamePlayScene::Finalize()
{

}

void GamePlayScene::Update()
{
	//GameObjectManagerの更新
	gameObjectManager_->Update();

	//トランジションの更新
	transition_->Update();

	//ロックオンの処理
	Enemy* enemy = gameObjectManager_->GetGameObject<Enemy>();
	lockOn_->Update(enemy, camera_);

	//FollowCameraの更新
	followCamera_->Update();

	//カメラの更新
	camera_ = followCamera_->GetCamera();
	camera_.UpdateMatrix();

	//フェードイン処理
	HandleTransition();

	//衝突判定
	collisionManager_->ClearColliderList();
	if (Collider* collider = gameObjectManager_->GetGameObject<Player>()->GetComponent<Collider>())
	{
		collisionManager_->SetColliderList(collider);
	}
	if (Collider* collider = gameObjectManager_->GetGameObject<Enemy>()->GetComponent<Collider>())
	{
		collisionManager_->SetColliderList(collider);
	}
	if (Collider* collider = gameObjectManager_->GetGameObject<Weapon>()->GetComponent<Collider>())
	{
		collisionManager_->SetColliderList(collider);
	}
	collisionManager_->CheckAllCollisions();

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

	//ロックオンの描画
	lockOn_->Draw();

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
		//Kキーを押したらGameClearSceneに遷移
		if (input_->IsPushKeyEnter(DIK_K))
		{
			transition_->SetFadeState(Transition::FadeState::In);
			nextScene_ = kGameClearScene;
		}
		//Lキーを押したらGameOverSceneに遷移
		else if (input_->IsPushKeyEnter(DIK_L))
		{
			transition_->SetFadeState(Transition::FadeState::In);
			nextScene_ = kGameClearScene;
		}
	}

	//シーン遷移
	if (transition_->GetFadeInComplete())
	{
		switch (nextScene_)
		{
		case kGameClearScene:
			sceneManager_->ChangeScene("GameClearScene");
			break;
		case kGameOverScene:
			sceneManager_->ChangeScene("GameOverScene");
			break;
		}
	}
}