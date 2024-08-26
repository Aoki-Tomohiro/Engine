#include "GamePlayScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Components/Animator/AnimatorComponent.h"

void GamePlayScene::Initialize()
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

	//衝突マネージャーの生成
	collisionManager_ = std::make_unique<CollisionManager>();

	//レベルデータの読み込み
	LevelLoader::Load("GameScene");

	//カメラを設定
	camera_ = CameraManager::GetInstance()->GetCamera("Camera");
	gameObjectManager_->SetCamera(camera_);
	particleManager_->SetCamera(camera_);

	//ロックオンの生成
	lockon_ = std::make_unique<Lockon>();
	lockon_->Initialize();

	//プレイヤーの初期化
	Player* player = gameObjectManager_->GetMutableGameObject<Player>("");
	player->SetCamera(camera_);
	player->SetLockon(lockon_.get());
	//アニメーターコンポーネントを追加
	AnimatorComponent* animatorComponent = player->AddComponent<AnimatorComponent>();
	animatorComponent->AddAnimation("Idle", AnimationManager::Create("Player/Animations/Idle.gltf"));
	animatorComponent->AddAnimation("Walk1", AnimationManager::Create("Player/Animations/Walk1.gltf"));
	animatorComponent->AddAnimation("Walk2", AnimationManager::Create("Player/Animations/Walk2.gltf"));
	animatorComponent->AddAnimation("Walk3", AnimationManager::Create("Player/Animations/Walk3.gltf"));
	animatorComponent->AddAnimation("Walk4", AnimationManager::Create("Player/Animations/Walk4.gltf"));
	animatorComponent->AddAnimation("Run1", AnimationManager::Create("Player/Animations/Run1.gltf"));
	animatorComponent->AddAnimation("Run2", AnimationManager::Create("Player/Animations/Run2.gltf"));
	animatorComponent->AddAnimation("Run3", AnimationManager::Create("Player/Animations/Run3.gltf"));
	animatorComponent->AddAnimation("Run4", AnimationManager::Create("Player/Animations/Run4.gltf"));
	animatorComponent->AddAnimation("Jump1", AnimationManager::Create("Player/Animations/Jump1.gltf"));
	animatorComponent->AddAnimation("Jump2", AnimationManager::Create("Player/Animations/Jump2.gltf"));
	animatorComponent->AddAnimation("Dodge1", AnimationManager::Create("Player/Animations/Dodge1.gltf"));
	animatorComponent->AddAnimation("Dodge2", AnimationManager::Create("Player/Animations/Dodge2.gltf"));
	animatorComponent->AddAnimation("Dodge3", AnimationManager::Create("Player/Animations/Dodge3.gltf"));
	animatorComponent->AddAnimation("Dodge4", AnimationManager::Create("Player/Animations/Dodge4.gltf"));
	animatorComponent->AddAnimation("DashStart", AnimationManager::Create("Player/Animations/DashStart.gltf"));
	animatorComponent->AddAnimation("DashEnd", AnimationManager::Create("Player/Animations/DashEnd.gltf"));

	//カメラパスマネージャーの生成
	cameraPathManager_ = std::make_unique<CameraPathManager>();
	cameraPathManager_->Initialize();

	//カメラコントローラーの生成
	cameraController_ = std::make_unique<CameraController>();
	cameraController_->Initialize();
	cameraController_->SetCameraPathManager(cameraPathManager_.get());
	cameraController_->SetLockon(lockon_.get());
	cameraController_->SetTarget(&player->GetComponent<ModelComponent>()->GetModel()->GetJointWorldTransform("mixamorig:Hips"));

	//トランジションの生成
	transition_ = std::make_unique<Transition>();
	transition_->Initialize();
}

void GamePlayScene::Finalize()
{

}

void GamePlayScene::Update()
{
	//トランジションの更新
	transition_->Update();

	//ゲームオブジェクトマネージャーの更新
	gameObjectManager_->Update();

	//ロックオンの更新
	lockon_->Update(camera_);

	//コライダーをクリア
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
	//衝突判定
	collisionManager_->CheckAllCollisions();

	//カメラパスマネージャーの更新
	cameraPathManager_->Update();

	//カメラコントローラーの更新
	cameraController_->Update();

	//シャドウマップ用のカメラを移動させる
	Camera* shadowCamera = CameraManager::GetCamera("ShadowCamera");
	shadowCamera->translation_ = { camera_->translation_.x,shadowCamera->translation_.y,camera_->translation_.z };

	//カメラの更新
	*camera_ = cameraController_->GetCamera();
	camera_->TransferMatrix();
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
	lockon_->Draw();

	//トランジションの描画
	transition_->Draw();

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}