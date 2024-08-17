#include "GamePlayScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Components/Collision/OBBCollider.h"
#include "Engine/Components/Collision/CollisionAttributeManager.h"

void GamePlayScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//GameObjectの初期化
	gameObjectManager_ = GameObjectManager::GetInstance();
	gameObjectManager_->Clear();

	//ParticleManagerを初期化
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Clear();

	//LevelDataの読み込み
	LevelLoader::Load("GameScene");

	//カメラを取得
	camera_ = CameraManager::GetCamera("Camera");
	//カメラを設定
	gameObjectManager_->SetCamera(camera_);
	particleManager_->SetCamera(camera_);

	//AnimationPhaseManagerの作成
	animationStateManager_ = std::make_unique<AnimationStateManager>();
	animationStateManager_->Initialize();

	//LockOnの生成
	lockon_ = std::make_unique<Lockon>();
	lockon_->Initialize();

	//プレイヤーの初期化
	Player* player = gameObjectManager_->GetMutableGameObject<Player>("");
	//カメラとロックオンを設定
	player->SetCamera(camera_);
	player->SetLockon(lockon_.get());
	player->SetAnimationStateManager(animationStateManager_.get());

	//武器の生成
	Weapon* weapon = GameObjectManager::CreateGameObject<Weapon>();
	//トランスフォームを追加
	TransformComponent* weaponTransformComponent = weapon->AddComponent<TransformComponent>();
	weaponTransformComponent->worldTransform_.translation_ = { 0.5f, -0.2f, 0.1f };
	weaponTransformComponent->worldTransform_.rotation_ = { 0.0f, 0.0f, 1.0f };
	weaponTransformComponent->worldTransform_.scale_ = { 4.0f, 4.0f, 4.0f };
	weaponTransformComponent->worldTransform_.parent_ = &player->GetComponent<ModelComponent>()->GetModel()->GetAnimation()->GetJointWorldTransform("mixamorig:RightHand");
	//モデルを追加
	ModelComponent* weaponModelComponent = weapon->AddComponent<ModelComponent>();
	weaponModelComponent->SetTransformComponent(weaponTransformComponent);
	weaponModelComponent->SetModel(ModelManager::CreateFromModelFile("PlayerWeapon", Opaque));
	//コライダーを追加
	CollisionAttributeManager* collisionAttributeManager = CollisionAttributeManager::GetInstance();
	OBBCollider* obbCollider = weapon->AddComponent<OBBCollider>();
	obbCollider->SetTransformComponent(player->GetComponent<TransformComponent>());
	obbCollider->SetCollisionAttribute(collisionAttributeManager->GetAttribute("PlayerWeapon"));
	obbCollider->SetCollisionMask(collisionAttributeManager->GetMask("PlayerWeapon"));

	//CameraPathManagerの作成
	cameraPathManager_ = std::make_unique<CameraPathManager>();
	cameraPathManager_->Initialize();

	//CameraControllerの作成
	cameraController_ = std::make_unique<CameraController>();
	cameraController_->Initialize();
	cameraController_->SetCameraPathManager(cameraPathManager_.get());
	cameraController_->SetLockon(lockon_.get());
	cameraController_->SetTarget(&player->GetComponent<ModelComponent>()->GetModel()->GetAnimation()->GetJointWorldTransform("mixamorig:Hips"));

	//CollisionManagerの生成
	collisionManager_ = std::make_unique<CollisionManager>();
}

void GamePlayScene::Finalize()
{

}

void GamePlayScene::Update()
{
	//パリィの処理
	UpdateParry();

	//ヒットストップの処理
	UpdateHitStop();

	//GameObjectManagerの更新
	gameObjectManager_->Update();

	//ロックオンの処理
	lockon_->Update(camera_);

	//衝突判定
	collisionManager_->ClearColliderList();
	//プレイヤーを衝突マネージャーに追加
	if (Player* player = gameObjectManager_->GetMutableGameObject<Player>(""))
	{
		if (Collider* collider = player->GetComponent<Collider>())
		{
			collisionManager_->SetColliderList(collider);
		}
	}
	//敵を衝突マネージャーに追加
	if (Enemy* enemy = gameObjectManager_->GetMutableGameObject<Enemy>(""))
	{
		if (Collider* collider = enemy->GetComponent<Collider>())
		{
			collisionManager_->SetColliderList(collider);
		}
	}
	//武器を衝突マネージャーに追加
	if (Weapon* weapon = gameObjectManager_->GetMutableGameObject<Weapon>("PlayerWeapon"))
	{
		if (Collider* collider = weapon->GetComponent<Collider>())
		{
			collisionManager_->SetColliderList(collider);
		}
	}
	collisionManager_->CheckAllCollisions();

	//CameraPathManagerの更新
	cameraPathManager_->Update();

	//CameraControllerの更新
	cameraController_->Update();

	//シャドウマップ用のカメラを移動させる
	Camera* shadowCamera = CameraManager::GetCamera("ShadowCamera");
	shadowCamera->translation_ = { camera_->translation_.x,shadowCamera->translation_.y,camera_->translation_.z };

	//カメラの更新
	*camera_ = cameraController_->GetCamera();
	camera_->TransferMatrix();

	//AnimationPhaseManagerの更新
	animationStateManager_->Update();
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

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void GamePlayScene::UpdateHitStop()
{
	//ヒットストップとパリィのスロウモーションが無効の場合
	if (!hitStopSettings_.isActive && !parrySettings_.isSlow)
	{
		//プレイヤーの攻撃がヒットしていたらヒットストップを有効化する
		if (const Weapon* weapon = gameObjectManager_->GetConstGameObject<Weapon>("PlayerWeapon"))
		{
			if (weapon->GetIsHit())
			{
				//ヒットストップを有効にする
				hitStopSettings_.isActive = true;
				GameTimer::SetTimeScale(0.0f);
			}
		}
	}

	//ヒットストップが有効の場合
	if (hitStopSettings_.isActive)
	{
		//タイマーを進める
		const float kHitStopDeltaTime = 1.0f / 60.0f;
		hitStopSettings_.timer += kHitStopDeltaTime;

		//ヒットストップのタイマーが指定の時間を超えたら無効化する
		if (hitStopSettings_.timer > hitStopSettings_.duration)
		{
			hitStopSettings_.isActive = false;
			hitStopSettings_.timer = 0.0f;
			GameTimer::SetTimeScale(1.0f);
		}
	}
}

void GamePlayScene::UpdateParry()
{
}