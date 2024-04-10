#include "GameClearScene.h"
#include "Engine/Framework/Scene/SceneManager.h"

void GameClearScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//PostEffectsのインスタンスを取得
	postEffects_ = PostEffects::GetInstance();

	//テクスチャの読み込み
	TextureManager::Load("circle.png");
	TextureManager::Load("Heart.png");

	//カメラの初期化
	camera_.Initialize();

	//パーティクルをクリア
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Clear();

	//パーティクルシステムの生成
	circleParticleSystem_ = ParticleManager::Create("Circle");
	circleParticleSystem_->SetTexture("circle.png");
	heartParticleSystem_ = ParticleManager::Create("Heart");
	heartParticleSystem_->SetTexture("Heart.png");
}

void GameClearScene::Finalize()
{

}

void GameClearScene::Update()
{
	if (input_->IsControllerConnected())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			sceneManager_->ChangeScene("GameTitleScene");
		}
	}

	if (input_->IsPushKeyEnter(DIK_SPACE))
	{
		sceneManager_->ChangeScene("GameTitleScene");
	}

	if (input_->IsControllerConnected())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
		{
			ParticleEmitter* newEmitter = ParticleEmitterBuilder()
				.SetDeleteTime(10.0f)
				.SetEmitterName("Test")
				.SetPopAzimuth(0.0f, 360.0f)
				.SetPopColor({ 0.0f,0.0f,0.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f })
				.SetPopCount(10)
				.SetPopElevation(0.0f, 0.0f)
				.SetPopFrequency(0.1f)
				.SetPopLifeTime(0.2f, 0.4f)
				.SetPopScale({ 0.2f,0.2f,0.2f }, { 0.4f,0.4f,0.4f })
				.SetPopVelocity({ 0.06f,0.06f,0.06f }, { 0.1f,0.1f,0.1f })
				.SetTranslation({ -6.0f,0.0f,0.0f })
				.Build();
			circleParticleSystem_->AddParticleEmitter(newEmitter);
		}

		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y))
		{
			ParticleEmitter* newEmitter = ParticleEmitterBuilder()
				.SetDeleteTime(10.0f)
				.SetEmitterName("Test")
				.SetPopAzimuth(0.0f, 360.0f)
				.SetPopColor({ 0.0f,0.0f,0.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f })
				.SetPopCount(10)
				.SetPopElevation(0.0f, 0.0f)
				.SetPopFrequency(0.1f)
				.SetPopLifeTime(0.2f, 0.4f)
				.SetPopScale({ 0.2f,0.2f,0.2f }, { 0.4f,0.4f,0.4f })
				.SetPopVelocity({ 0.06f,0.06f,0.06f }, { 0.1f,0.1f,0.1f })
				.SetTranslation({ 6.0f,0.0f,0.0f })
				.Build();
			heartParticleSystem_->AddParticleEmitter(newEmitter);
		}
	}

	//パーティクルの更新
	particleManager_->Update();

	ImGui::Begin("GameClearScene");
	ImGui::Text("Space or AButton : GameTitleScene");
	ImGui::Text("B : CircleParticle");
	ImGui::Text("Y : HeartParticle");

	const char* textureName[] = { "Circle", "Heart" };
	if (ImGui::TreeNode("CircleParticle"))
	{
		static int currentTextureNum = 0;
		if (ImGui::Combo("Texture", &currentTextureNum, textureName, IM_ARRAYSIZE(textureName)))
		{
			if (currentTextureNum == 0)
			{
				circleParticleSystem_->SetTexture("circle.png");
			}
			else
			{
				circleParticleSystem_->SetTexture("Heart.png");
			}
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("HeartParticle"))
	{
		static int currentTextureNum = 0;
		if (ImGui::Combo("Texture", &currentTextureNum, textureName, IM_ARRAYSIZE(textureName)))
		{
			if (currentTextureNum == 0)
			{
				heartParticleSystem_->SetTexture("circle.png");
			}
			else
			{
				heartParticleSystem_->SetTexture("Heart.png");
			}
		}
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

void GameClearScene::Draw()
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
	//3Dオブジェクト描画
	renderer_->Render();
#pragma endregion

#pragma region パーティクル描画
	//パーティクル描画前処理
	renderer_->PreDrawParticles();

	//パーティクルの描画
	particleManager_->Draw(camera_);

	//パーティクル描画後処理
	renderer_->PostDrawParticles();
#pragma endregion
}

void GameClearScene::DrawUI()
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}