#include "GameTitleScene.h"
#include "Engine/Framework/Scene/SceneManager.h"

void GameTitleScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//PostEffectsのインスタンスを取得
	postEffects_ = PostEffects::GetInstance();

	//テクスチャ読み込み
	TextureManager::Load("uvChecker.png");

	//スプライトの生成
	for (uint32_t i = 0; i < 2; ++i)
	{
		sprites_.push_back(std::unique_ptr<Sprite>(Sprite::Create("uvChecker.png", { i * 768.0f,0.0f })));
	}

	//音声データの読み込みと再生
	audioHandle_ = audio_->SoundLoadWave("Application/Resources/Sounds/Sample.wav");
	audio_->StopAudio(audioHandle_);
	audio_->SoundPlayWave(audioHandle_, true, 0.5f);
}

void GameTitleScene::Finalize()
{

}

void GameTitleScene::Update()
{
	//シーン切り替え処理
	if (input_->IsControllerConnected())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			sceneManager_->ChangeScene("GamePlayScene");
		}
	}

	if (input_->IsPushKeyEnter(DIK_SPACE))
	{
		sceneManager_->ChangeScene("GamePlayScene");
	}

	ImGui::Begin("GameTitleScene");
	ImGui::Text("Space or AButton : GameTitleScene");
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

	//スプライトの描画
	for (const std::unique_ptr<Sprite>& sprite : sprites_)
	{
		sprite->Draw();
	}

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}