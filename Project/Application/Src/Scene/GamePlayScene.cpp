#include "GamePlayScene.h"
#include "Engine/Framework/Scene/SceneManager.h"

void GamePlayScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//PostEffectsのインスタンスを取得
	postEffects_ = PostEffects::GetInstance();

	//カメラの初期化
	camera_.Initialize();
	camera_.translation_ = { 0.0f,20.0f,-50.0f };
	camera_.rotation_ = { 0.4f,0.0f,0.0f };

	//ゲームオブジェクトをクリア
	gameObjectManager_ = GameObjectManager::GetInstance();
	gameObjectManager_->Clear();

	//ライトマネージャーのインスタンスを取得
	lightManager_ = LightManager::GetInstance();

	//モデルの生成
	modelPlayer_.reset(ModelManager::CreateFromOBJ("Player", Opaque));
	groundModel_.reset(ModelManager::Create());
	groundModel_->SetColor({ 0.6f,0.6f,0.6f,1.0f });

	//ゲームオブジェクトの生成
	player_ = GameObjectManager::CreateGameObject<Player>();
	player_->SetModel(modelPlayer_.get());
	ground_ = GameObjectManager::CreateGameObject<Ground>();
	ground_->SetModel(groundModel_.get());

	//デバッグカメラの生成
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();
	debugCamera_->SetTarget(&player_->GetWorldTransform());
}

void GamePlayScene::Finalize()
{

}

void GamePlayScene::Update()
{
	//シーン切り替え
	if (input_->IsControllerConnected())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			sceneManager_->ChangeScene("GameClearScene");
		}
	}

	if (input_->IsPushKeyEnter(DIK_SPACE))
	{
		sceneManager_->ChangeScene("GameClearScene");
	}

	//ゲームオブジェクトの更新
	gameObjectManager_->Update();

	//カメラの更新
	if (isDebugCameraActive_)
	{
		//デバッグカメラの更新
		debugCamera_->Update();
		camera_.matView_ = debugCamera_->GetViewProjection().matView_;
		camera_.matProjection_ = debugCamera_->GetViewProjection().matProjection_;
		camera_.TransferMatrix();
	}
	else
	{
		camera_.UpdateMatrix();
	}

	//ライトの更新
	lightManager_->GetDirectionalLight(0).SetIsEnable(isDirectionalLightEnable_);
	lightManager_->GetDirectionalLight(0).SetColor(directionalLightData_.color);
	lightManager_->GetDirectionalLight(0).SetDirection(directionalLightData_.direction);
	lightManager_->GetDirectionalLight(0).SetIntensity(directionalLightData_.intensity);
	lightManager_->GetPointLight(0).SetIsEnable(isPointLightEnable_);
	lightManager_->GetPointLight(0).SetColor(pointLightData_.color);
	lightManager_->GetPointLight(0).SetPosition(pointLightData_.position);
	lightManager_->GetPointLight(0).SetIntensity(pointLightData_.intensity);
	lightManager_->GetPointLight(0).SetRadius(pointLightData_.radius);
	lightManager_->GetPointLight(0).SetDecay(pointLightData_.decay);
	lightManager_->GetSpotLight(0).SetIsEnable(isSpotLightEnable_);
	lightManager_->GetSpotLight(0).SetColor(spotLightData_.color);
	lightManager_->GetSpotLight(0).SetPosition(spotLightData_.position);
	lightManager_->GetSpotLight(0).SetIntensity(spotLightData_.intensity);
	lightManager_->GetSpotLight(0).SetDirection(spotLightData_.direction);
	lightManager_->GetSpotLight(0).SetDistance(spotLightData_.distance);
	lightManager_->GetSpotLight(0).SetDecay(spotLightData_.decay);
	lightManager_->GetSpotLight(0).SetCosFalloffStart(spotLightData_.cosFalloffStart);
	lightManager_->GetSpotLight(0).SetCosAngle(spotLightData_.cosAngle);

	//ImGui
	ImGui::Begin("GamePlayScene");
	ImGui::Text("Space or AButton : GameClearScene");
	ImGui::Text("LStick : Move");
	if (ImGui::TreeNode("DebugCamera"))
	{
		ImGui::Checkbox("IsDebugCameraActive", &isDebugCameraActive_);
		ImGui::Text("WASD : Upward, Downward, Leftward, And Rightward");
		ImGui::Text("ScrollForward ScrollBackward : Forward And Backward");
		ImGui::Text("ArrowKey : Rotate");
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("DirectionalLight"))
	{
		ImGui::Checkbox("IsDirectionalLightEnable", &isDirectionalLightEnable_);
		ImGui::ColorEdit3("Color", &directionalLightData_.color.x);
		ImGui::DragFloat3("Direction", &directionalLightData_.direction.x, 0.01f);
		ImGui::DragFloat("Intensity", &directionalLightData_.intensity, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("PointLight"))
	{
		ImGui::Checkbox("IsPointLightEnable", &isPointLightEnable_);
		ImGui::ColorEdit3("Color", &pointLightData_.color.x);
		ImGui::DragFloat3("Position", &pointLightData_.position.x, 0.01f);
		ImGui::DragFloat("Intensity", &pointLightData_.intensity, 0.01f);
		ImGui::DragFloat("Radius", &pointLightData_.radius, 0.1f);
		ImGui::DragFloat("Decay", &pointLightData_.decay, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("SpotLight"))
	{
		ImGui::Checkbox("IsSpotLightEnable", &isSpotLightEnable_);
		ImGui::ColorEdit3("Color", &spotLightData_.color.x);
		ImGui::DragFloat3("Position", &spotLightData_.position.x, 0.01f);
		ImGui::DragFloat("Intensity", &spotLightData_.intensity, 0.01f);
		ImGui::DragFloat3("Direction", &spotLightData_.direction.x, 0.01f);
		ImGui::DragFloat("Distance", &spotLightData_.distance, 0.1f);
		ImGui::DragFloat("Decay", &spotLightData_.decay, 0.01f);
		ImGui::DragFloat("CosFalloffStart", &spotLightData_.cosFalloffStart, 0.01f);
		ImGui::DragFloat("CosAngle", &spotLightData_.cosAngle, 0.01f);
		ImGui::TreePop();
	}
	ImGui::End();

	//Bloomの更新
	postEffects_->GetBloom()->SetIsEnable(isBloomEnable_);
	postEffects_->GetBloom()->SetIntensity(bloomData_.intensity);
	postEffects_->GetBloom()->SetBlurCount(blurCount_);
	postEffects_->GetBloom()->SetTextureWeight(bloomData_.textureWeight);
	postEffects_->GetBloom()->SetHighLumTextureWeight(bloomData_.highLumTextureWeight);
	for (uint32_t i = 0; i < 4; ++i)
	{
		postEffects_->GetBloom()->SetBlurTextureWeight(0, bloomData_.blurTextureWeight[i]);
	}

	//Fogの更新
	postEffects_->GetFog()->SetIsEnable(isFogEnable_);
	postEffects_->GetFog()->SetScale(fogData_.scale);
	postEffects_->GetFog()->SetAttenuationRate(fogData_.attenuationRate);

	//DoFの更新
	postEffects_->GetDepthOfField()->SetIsEnable(isDoFEnable_);
	postEffects_->GetDepthOfField()->SetFocusDepth(dofData_.focusDepth);
	postEffects_->GetDepthOfField()->SetNFocusWidth(dofData_.nFocusWidth);
	postEffects_->GetDepthOfField()->SetFFocusWidth(dofData_.fFocusWidth);

	//LensDistortionの更新
	postEffects_->GetLensDistortion()->SetIsEnable(isLensDistortionEnable_);
	postEffects_->GetLensDistortion()->SetTightness(lensDistortionData_.tightness);
	postEffects_->GetLensDistortion()->SetStrength(lensDistortionData_.strength);

	//Vignetteの更新
	postEffects_->GetVignette()->SetIsEnable(isVignetteEnable_);
	postEffects_->GetVignette()->SetIntensity(vignetteData_.isEnable);

	ImGui::Begin("PostEffects");
	if (ImGui::TreeNode("Bloom"))
	{
		ImGui::Checkbox("IsEnable", &isBloomEnable_);
		ImGui::SliderInt("BlurCount", &blurCount_, 0, 4);
		ImGui::DragFloat("Intensity", &bloomData_.intensity, 0.01f);
		ImGui::DragFloat("TextureWeight", &bloomData_.textureWeight, 0.01f);
		ImGui::DragFloat("HighLumTextureWeight", &bloomData_.highLumTextureWeight, 0.01f);
		ImGui::DragFloat("BlurTextureWeight1", &bloomData_.blurTextureWeight[0], 0.01f);
		ImGui::DragFloat("BlurTextureWeight2", &bloomData_.blurTextureWeight[1], 0.01f);
		ImGui::DragFloat("BlurTextureWeight3", &bloomData_.blurTextureWeight[2], 0.01f);
		ImGui::DragFloat("BlurTextureWeight4", &bloomData_.blurTextureWeight[3], 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Fog"))
	{
		ImGui::Checkbox("IsEnable", &isFogEnable_);
		ImGui::DragFloat("Scale", &fogData_.scale, 0.01f);
		ImGui::DragFloat("AttenuationRate", &fogData_.attenuationRate, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("DoF"))
	{
		ImGui::Checkbox("IsEnable", &isDoFEnable_);
		ImGui::DragFloat("FocusDepth", &dofData_.focusDepth, 0.01f);
		ImGui::DragFloat("nFocusWidth", &dofData_.nFocusWidth, 0.01f);
		ImGui::DragFloat("fFocusWidth", &dofData_.fFocusWidth, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("LensDistortion"))
	{
		ImGui::Checkbox("IsEnable", &isLensDistortionEnable_);
		ImGui::DragFloat("Tightness", &lensDistortionData_.tightness, 0.01f);
		ImGui::SliderFloat("Strength", &lensDistortionData_.strength, -0.1f, 0.1f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Vignette"))
	{
		ImGui::Checkbox("IsEnable", &isVignetteEnable_);
		ImGui::DragFloat("Intensity", &vignetteData_.intensity, 0.1f);
		ImGui::TreePop();
	}
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

#pragma region 3Dオブジェクト描画
	//ゲームオブジェクトの描画
	gameObjectManager_->Draw(camera_);

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

void GamePlayScene::DrawUI()
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}