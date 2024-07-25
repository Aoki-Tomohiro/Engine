#include "GamePlayScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/LevelLoader/LevelLoader.h"
#include "Engine/Utilities/GameTimer.h"
#include "Engine/Utilities/RandomGenerator.h"
#include "Engine/Utilities/GlobalVariables.h"
#include <numbers>

void GamePlayScene::Initialize()
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
	LevelLoader::Load("GameScene");

	//カメラを取得
	camera_ = CameraManager::GetInstance()->GetCamera("Camera");
	//GameObjectManagerにカメラを設定
	gameObjectManager_->SetCamera(camera_);
	//ParticleManagerにカメラを設定
	particleManager_->SetCamera(camera_);

	//LockOnの生成
	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();

	//プレイヤーの初期化
	Player* player = gameObjectManager_->GetGameObject<Player>();
	//TransformComponentの初期化
	TransformComponent* playerTransformComponent = player->GetComponent<TransformComponent>();
	playerTransformComponent->worldTransform_.rotationType_ = RotationType::Quaternion;
	//ModelComponentの初期化
	ModelComponent* playerModelComponent = player->GetComponent<ModelComponent>();
	playerModelComponent->SetAnimationName("Armature|mixamo.com|Layer0");
	//カメラとロックオンを設定
	player->SetCamera(camera_);
	player->SetLockOn(lockOn_.get());

	//敵の初期化
	Enemy* enemy = gameObjectManager_->GetGameObject<Enemy>();
	enemy->SetCamera(camera_);
	//TransformComponentの初期化
	TransformComponent* enemyTransformComponent = enemy->GetComponent<TransformComponent>();
	enemyTransformComponent->worldTransform_.rotationType_ = RotationType::Quaternion;
	enemy->SetDestinationQuaternion(enemyTransformComponent->worldTransform_.quaternion_);
	//ModelComponentの初期化
	ModelComponent* enemyModelComponent = enemy->GetComponent<ModelComponent>();
	enemyModelComponent->SetAnimationName("Armature|mixamo.com|Layer0");

	//武器の生成
	Weapon* weapon = gameObjectManager_->GetGameObject<Weapon>();
	//プレイヤーを親に設定
	weapon->SetParent(&playerModelComponent->GetModel()->GetAnimation()->GetJointWorldTransform("mixamorig:RightHand"));

	//FollowCameraの作成
	cameraController_ = std::make_unique<CameraController>();
	cameraController_->Initialize();
	cameraController_->SetTarget(&playerTransformComponent->worldTransform_);
	cameraController_->SetLockOn(lockOn_.get());

	//トランジションの生成
	transition_ = std::make_unique<Transition>();
	transition_->Initialize();

	//Skyboxの初期化
	skybox_.reset(Skybox::Create());

	//CollisionManagerの生成
	collisionManager_ = std::make_unique<CollisionManager>();

	//ゲームオーバーのスプライトの生成
	TextureManager::Load("GameOver.png");
	gameOverSprite_.reset(Sprite::Create("GameOver.png", { 0.0f,0.0f }));

	//ゲームクリアのスプライトの生成
	TextureManager::Load("GameClear.png");
	gameClearSprite_.reset(Sprite::Create("GameClear.png", { 0.0f,0.0f }));

	//環境変数の設定
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "GameScene";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "HitStopDuration", hitStopSettings_.duration);
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

	//トランジションの更新
	transition_->Update();

	//ロックオンの処理
	Enemy* enemy = gameObjectManager_->GetGameObject<Enemy>();
	lockOn_->SetTargetOffset({ 0.0f,5.0f,0.0f });
	lockOn_->Update(enemy, camera_);

	//衝突判定
	collisionManager_->ClearColliderList();
	Player* player = gameObjectManager_->GetGameObject<Player>();
	if (Collider* collider = player->GetComponent<Collider>())
	{
		collisionManager_->SetColliderList(collider);
	}
	if (Collider* collider = enemy->GetComponent<Collider>())
	{
		collisionManager_->SetColliderList(collider);
	}
	Weapon* weapon = gameObjectManager_->GetGameObject<Weapon>();
	if (Collider* collider = weapon->GetComponent<Collider>())
	{
		collisionManager_->SetColliderList(collider);
	}
	std::vector<MagicProjectile*> magicProjectiles = gameObjectManager_->GetGameObjects<MagicProjectile>();
	for (MagicProjectile* magicProjectil : magicProjectiles)
	{
		if (Collider* collider = magicProjectil->GetComponent<Collider>())
		{
			collisionManager_->SetColliderList(collider);
		}
	}
	std::vector<Warning*> warnings = gameObjectManager_->GetGameObjects<Warning>();
	for (Warning* warning : warnings)
	{
		if (Collider* collider = warning->GetComponent<Collider>())
		{
			collisionManager_->SetColliderList(collider);
		}
	}
	collisionManager_->CheckAllCollisions();

	//CameraControllerの更新
	cameraController_->Update();

	//カメラの更新
	*camera_ = cameraController_->GetCamera();
	camera_->TransferMatrix();

	//フェードイン処理
	HandleTransition();

	//グローバル変数の適用
	ApplyGlobalVariables();

	////HSV
	//PostEffects* postEffects = PostEffects::GetInstance();
	//postEffects->GetHSV()->SetHue(hue_);
	//postEffects->GetHSV()->SetSaturation(saturation_);
	//postEffects->GetHSV()->SetValue(value_);

	//ImGui
	ImGui::Begin("GamePlayScene");
	ImGui::Text("K : GameClearScene");
	ImGui::Text("L : GameOverScene");
	ImGui::DragFloat("Hue", &hue_, 0.02f, -0.5f, 0.5f);
	ImGui::DragFloat("Saturation", &saturation_, 0.02f, -0.5f, 0.5f);
	ImGui::DragFloat("Value", &value_, 0.02f, -0.5f, 0.5f);
	ImGui::End();
}

void GamePlayScene::Draw()
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

void GamePlayScene::DrawUI()
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//GameObjectManagerのUIを描画
	gameObjectManager_->DrawUI();

	//ロックオンの描画
	lockOn_->Draw();

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

void GamePlayScene::HandleTransition()
{
	////FadeInしていないとき
	//if (transition_->GetFadeState() != transition_->FadeState::In)
	//{
	//	Player* player = gameObjectManager_->GetGameObject<Player>();
	//	Enemy* enemy = gameObjectManager_->GetGameObject<Enemy>();
	//	//Kキーを押したらGameClearSceneに遷移
	//	if (input_->IsPushKeyEnter(DIK_K) || enemy->GetHP() <= 0.0f)
	//	{
	//		isGameClear_ = true;
	//		player->SetIsInTitleScene(true);
	//		enemy->SetIsInTitleScene(true);
	//		ModelComponent* modelComponent = enemy->GetComponent<ModelComponent>();
	//		modelComponent->SetAnimationName("Idle");
	//	}
	//	//Lキーを押したらGameOverSceneに遷移
	//	else if (input_->IsPushKeyEnter(DIK_L) || player->GetHP() <= 0.0f)
	//	{
	//		isGameOver_ = true;
	//		player->SetIsInTitleScene(true);
	//		enemy->SetIsInTitleScene(true);
	//		ModelComponent* modelComponent = enemy->GetComponent<ModelComponent>();
	//		modelComponent->SetAnimationName("Idle");
	//	}
	//}

	////ゲームクリアかゲームオーバーの時のどちらかになっていたらタイトルに戻る
	//if (isGameClear_ || isGameOver_)
	//{
	//	if (input_->IsPressButton(XINPUT_GAMEPAD_A))
	//	{
	//		transition_->SetFadeState(Transition::FadeState::In);
	//	}
	//}

	////シーン遷移
	//if (transition_->GetFadeInComplete())
	//{
	//	sceneManager_->ChangeScene("GameTitleScene");
	//}
}

void GamePlayScene::UpdateHitStop()
{
	//ヒットストップとパリィのスロウモーションが無効の場合
	if (!hitStopSettings_.isActive && !parrySettings_.isSlow)
	{
		//プレイヤーの攻撃がヒットしていたらヒットストップを有効化する
		if (gameObjectManager_->GetGameObject<Weapon>()->GetIsHit())
		{
			//ヒットストップを有効にする
			hitStopSettings_.isActive = true;
			GameTimer::SetTimeScale(0.0f);
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
	//プレイヤーがパリィに成功していた場合
	if (gameObjectManager_->GetGameObject<Weapon>()->GetIsParrySuccessful())
	{
		//パリィのフラグを立てる
		parrySettings_.isActive = true;

		//スロウモーションのフラグを立てる
		parrySettings_.isSlow = true;

		//敵の行動を遅くする
		gameObjectManager_->GetGameObject<Enemy>()->SetTimeScale(0.2f);

		//ゲーム全体の時間の流れを遅くする
		GameTimer::SetTimeScale(0.2f);
	}

	//パリィ状態の時
	if (parrySettings_.isActive)
	{
		//タイマーを進める
		const float kParryDeltaTime = 1.0f / 60.0f;
		parrySettings_.timer += kParryDeltaTime;

		//スロウモーション状態の時
		if (parrySettings_.isSlow)
		{
			//停止時間を超えていたら
			if (parrySettings_.timer > parrySettings_.stopDuration)
			{
				//スロウモーションのフラグを折る
				parrySettings_.isSlow = false;

				//ゲーム全体の時間の流れをもとに戻す
				GameTimer::SetTimeScale(1.0f);
			}
		}

		//パリィのタイマーが一定間隔を超えたら
		if (parrySettings_.timer > parrySettings_.duration)
		{
			//タイマーのリセット
			parrySettings_.timer = 0.0f;
			parrySettings_.grayTimer = 0.0f;

			//フラグのリセット
			parrySettings_.isActive = false;


			//敵の行動をもとに戻す
			gameObjectManager_->GetGameObject<Enemy>()->SetTimeScale(1.0f);
		}

		//GreeScaleにする
		PostEffects* postEffects = PostEffects::GetInstance();
		if (parrySettings_.timer < parrySettings_.grayDuration)
		{
			parrySettings_.grayTimer += kParryDeltaTime;
			float easingParameter = 1.0f * (parrySettings_.grayTimer / parrySettings_.grayDuration);
			float currentSaturation = 0.0f + (parrySettings_.graySaturation - 0.0f) * Mathf::EaseInSine(easingParameter);
			postEffects->GetHSV()->SetSaturation(currentSaturation);
		}
		else if (parrySettings_.timer > parrySettings_.duration - parrySettings_.grayDuration)
		{
			parrySettings_.grayTimer += kParryDeltaTime;
			float easingParameter = 1.0f * (parrySettings_.grayTimer / parrySettings_.grayDuration);
			float currentSaturation = parrySettings_.graySaturation + (0.0f - parrySettings_.graySaturation) * Mathf::EaseInSine(easingParameter);
			postEffects->GetHSV()->SetSaturation(currentSaturation);
		}
		else
		{
			parrySettings_.grayTimer = 0.0f;
		}
	}
}

void GamePlayScene::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "GameScene";
	hitStopSettings_.duration = globalVariables->GetFloatValue(groupName, "HitStopDuration");
}