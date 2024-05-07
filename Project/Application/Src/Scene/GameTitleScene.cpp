#include "GameTitleScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Base/TextureManager.h"
#include <numbers>

void GameTitleScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//ゲームオブジェクトをクリア
	gameObjectManager_ = GameObjectManager::GetInstance();
	gameObjectManager_->Clear();

	//パーティクルをクリア
	ParticleManager::GetInstance()->Clear();

	//カメラの初期化
	camera_.Initialize();
	camera_.translation_.y = 30.0f;
	camera_.translation_.z = -100.0f;
	//camera_.rotation_.x = 0.3f;

	//プレイヤーの生成
	playerModelHead_.reset(ModelManager::CreateFromModelFile("PlayerHead.obj", Opaque));
	playerModelHead_->GetMaterial()->SetEnableLighting(false);
	playerModelBody_.reset(ModelManager::CreateFromModelFile("PlayerBody.obj", Opaque));
	playerModelBody_->GetMaterial()->SetEnableLighting(false);
	playerModelL_Arm_.reset(ModelManager::CreateFromModelFile("PlayerL_arm.obj", Opaque));
	playerModelL_Arm_->GetMaterial()->SetEnableLighting(false);
	playerModelR_Arm_.reset(ModelManager::CreateFromModelFile("PlayerR_arm.obj", Opaque));
	playerModelR_Arm_->GetMaterial()->SetEnableLighting(false);

	//ワールドトランスフォームの初期化
	for (uint32_t i = 0; i < 5; ++i)
	{
		playerWorldTransforms[i].Initialize();
	}
	playerWorldTransforms[0].translation_.z = -20.0f;
	playerWorldTransforms[2].translation_ = { 0.0f,1.85f,0.0f };
	playerWorldTransforms[2].rotation_ = { 0.0f,0.0f,0.0f };
	playerWorldTransforms[3].translation_ = { -0.7f,1.8f,0.0f };
	playerWorldTransforms[3].rotation_ = { 0.0f,0.0f,0.0f };
	playerWorldTransforms[4].translation_ = { 0.7f,1.8f,0.0f };
	playerWorldTransforms[4].rotation_ = { 0.0f,0.0f,0.0f };

	//親子付け
	playerWorldTransforms[1].parent_ = &playerWorldTransforms[0];
	playerWorldTransforms[2].parent_ = &playerWorldTransforms[1];
	playerWorldTransforms[3].parent_ = &playerWorldTransforms[1];
	playerWorldTransforms[4].parent_ = &playerWorldTransforms[1];

	//ボスの生成
	bossModel_.reset(ModelManager::CreateFromModelFile("walk.gltf", Opaque));
	bossModel_->GetMaterial()->SetEnableLighting(false);
	bossModel_->GetMaterial()->SetColor({ 0.9f, 0.5f, 0.9f, 1.0f });
	bossWorldTransform_.Initialize();
	bossWorldTransform_.translation_.y = 3.0f;
	bossWorldTransform_.scale_ = { 3.0f,3.0f,3.0f };
	bossWorldTransform_.quaternion_ = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, std::numbers::pi_v<float>);

	//天球の作成
	skydomeModel_.reset(ModelManager::CreateFromModelFile("Skydome.obj", Opaque));
	skydomeModel_->GetMaterial()->SetEnableLighting(false);
	skydome_ = GameObjectManager::CreateGameObject<Skydome>();
	skydome_->SetModel(skydomeModel_.get());

	//地面の生成
	groundModel_.reset(ModelManager::CreateFromModelFile("Ground.obj", Opaque));
	groundModel_->GetMaterial()->SetEnableLighting(false);
	ground_ = GameObjectManager::CreateGameObject<Ground>();
	ground_->SetModel(groundModel_.get());

	//トランジションの初期化
	transitionSprite_.reset(Sprite::Create("white.png", { 0.0f,0.0f }));
	transitionSprite_->SetSize({ 1280.0f,720.0f });
	transitionSprite_->SetColor(transitionSpriteColor_);

	//タイトルのスプライトの生成
	TextureManager::Load("GameTitle.png");
	titleSprite_.reset(Sprite::Create("GameTitle.png", { 0.0f,0.0f }));
	TextureManager::Load("PressA.png");
	pressASprite_.reset(Sprite::Create("PressA.png", { 0.0f,0.0f }));

	//BGMの読み込みと再生
	bgmHandle_ = audio_->LoadAudioFile("Application/Resources/Sounds/Title.mp3");
	audio_->PlayAudio(bgmHandle_, true, 0.5f);
}

void GameTitleScene::Finalize()
{

}

void GameTitleScene::Update()
{
	//トランジションの更新
	UpdateTransition();

	//ゲームオブジェクトの更新
	gameObjectManager_->Update();

	//ワールドトランスフォームの更新
	for (uint32_t i = 0; i < 5; ++i)
	{
		playerWorldTransforms[i].UpdateMatrixFromEuler();
	}

	bossWorldTransform_.UpdateMatrixFromQuaternion();

	//カメラの更新
	const float kRotSpeed = 0.006f;
	camera_.rotation_.y += kRotSpeed;
	Matrix4x4 rotateYMatrix = Mathf::MakeRotateYMatrix(camera_.rotation_.y);
	//Vector3 offset = { 0.0f,30.0f ,-80.0f };
	Vector3 offset = { 0.0f,10.0f ,-60.0f };
	offset = Mathf::TransformNormal(offset, rotateYMatrix);
	camera_.translation_ = offset;
	camera_.UpdateMatrix();

	bossModel_->Update(bossWorldTransform_);
	if (input_->IsPushKeyEnter(DIK_1))
	{
		bossModel_->GetAnimation()->PlayRigidAnimation();
	}
	else if (input_->IsPushKeyEnter(DIK_2))
	{
		bossModel_->GetAnimation()->StopRigidAnimation();
	}
	else if (input_->IsPushKeyEnter(DIK_3))
	{
		bossModel_->GetAnimation()->PauseRigidAnimation();
	}
}

void GameTitleScene::Draw()
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
	////プレイヤーのモデルの描画
	//playerModelBody_->Draw(playerWorldTransforms[1], camera_);
	//playerModelHead_->Draw(playerWorldTransforms[2], camera_);
	//playerModelL_Arm_->Draw(playerWorldTransforms[3], camera_);
	//playerModelR_Arm_->Draw(playerWorldTransforms[4], camera_);

	//ボスのモデルの描画
	bossModel_->Draw(bossWorldTransform_, camera_);

	////ゲームオブジェクトのモデル描画
	//gameObjectManager_->Draw(camera_);

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

void GameTitleScene::DrawUI()
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	////タイトルのスプライトの描画
	//titleSprite_->Draw();

	////PressAのスプライトの描画
	//pressASprite_->Draw();

	////トランジション用のスプライトの描画
	//transitionSprite_->Draw();

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void GameTitleScene::UpdateTransition()
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
			sceneManager_->ChangeScene("GamePlayScene");
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