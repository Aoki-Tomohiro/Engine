#include "GameTitleScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"
#include <numbers>

void GameTitleScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	lightManager_ = LightManager::GetInstance();

	//カメラの初期化
	camera_.Initialize();

	//デバッグカメラの初期化
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();
	debugCamera_->SetTarget(&monsterBallWorldTransform_);

	//モデルの生成
	terrainModel_.reset(ModelManager::CreateFromOBJ("terrain", Opaque));
	monsterBallModel_.reset(ModelManager::CreateFromOBJ("MonsterBall", Opaque));

	//ワールドトランスフォームの初期化
	terrainWorldTransform_.Initialize();
	terrainWorldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	monsterBallWorldTransform_.Initialize();
	monsterBallWorldTransform_.rotation_.y = std::numbers::pi_v<float>;
}

void GameTitleScene::Finalize()
{

}

void GameTitleScene::Update()
{
	//ワールドトランスフォームの更新
	terrainWorldTransform_.UpdateMatrixFromEuler();
	monsterBallWorldTransform_.UpdateMatrixFromEuler();

	//デバッグカメラの更新
	debugCamera_->Update();

	//カメラの更新
	camera_.matView_ = debugCamera_->GetViewProjection().matView_;
	camera_.matProjection_ = debugCamera_->GetViewProjection().matProjection_;
	camera_.TransferMatrix();

	for (int i = 0; i < 2; ++i)
	{
		//DirectionalLightの更新
		lightManager_->GetDirectionalLight(i).SetIsEnable(isDirectionalLightEnable_[i]);
		lightManager_->GetDirectionalLight(i).SetColor(directionalLightData_[i].color);
		lightManager_->GetDirectionalLight(i).SetDirection(directionalLightData_[i].direction);
		lightManager_->GetDirectionalLight(i).SetIntensity(directionalLightData_[i].intensity);

		//PointLightの更新
		lightManager_->GetPointLight(i).SetIsEnable(isPointLightEnable_[i]);
		lightManager_->GetPointLight(i).SetColor(pointLightData_[i].color);
		lightManager_->GetPointLight(i).SetPosition(pointLightData_[i].position);
		lightManager_->GetPointLight(i).SetIntensity(pointLightData_[i].intensity);
		lightManager_->GetPointLight(i).SetRadius(pointLightData_[i].radius);
		lightManager_->GetPointLight(i).SetDecay(pointLightData_[i].decay);

		//SpotLightの更新
		lightManager_->GetSpotLight(i).SetIsEnable(isSpotLightEnable_[i]);
		lightManager_->GetSpotLight(i).SetColor(spotLightData_[i].color);
		lightManager_->GetSpotLight(i).SetPosition(spotLightData_[i].position);
		lightManager_->GetSpotLight(i).SetIntensity(spotLightData_[i].intensity);
		lightManager_->GetSpotLight(i).SetDirection(spotLightData_[i].direction);
		lightManager_->GetSpotLight(i).SetDistance(spotLightData_[i].distance);
		lightManager_->GetSpotLight(i).SetDecay(spotLightData_[i].decay);
		lightManager_->GetSpotLight(i).SetCosFalloffStart(spotLightData_[i].cosFalloffStart);
		lightManager_->GetSpotLight(i).SetCosAngle(spotLightData_[i].cosAngle);
	}

	//ImGui
	ImGui::Begin("GameTitleScene");

	if (ImGui::TreeNode("MonsterBall"))
	{
		ImGui::DragFloat3("Translation", &monsterBallWorldTransform_.translation_.x, 0.1f);
		ImGui::DragFloat3("Rotation", &monsterBallWorldTransform_.rotation_.x, 0.01f);
		ImGui::DragFloat3("Scale", &monsterBallWorldTransform_.scale_.x, 0.01f);
		ImGui::Checkbox("EnableLighting", &enableLighting_);
		ImGui::DragFloat("Shininess", &shininess_, 0.1f);
		const char* diffuseReflectionTypes[] = { "LambertianReflectance", "SpecularReflectionType" };
		if (ImGui::Combo("DiffuseReflectionType", reinterpret_cast<int*>(&diffuseReflectionType_), diffuseReflectionTypes, IM_ARRAYSIZE(diffuseReflectionTypes)))
		{
			monsterBallModel_->SetDiffuseReflectionType(diffuseReflectionType_);
		}
		const char* specularReflectionTypes[] = { "PhongReflectionModel", "BlinnPhongReflectionModel", "NoSpecularReflection" };
		if (ImGui::Combo("SpecularReflectionType", reinterpret_cast<int*>(&specularReflectionType_), specularReflectionTypes, IM_ARRAYSIZE(specularReflectionTypes)))
		{
			monsterBallModel_->SetSpecularReflectionType(specularReflectionType_);
		}
		ImGui::ColorEdit4("SpecularColor", &specularColor_.x);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Camera"))
	{
		ImGui::DragFloat3("Translation", &camera_.translation_.x, 0.1f);
		ImGui::DragFloat3("Rotation", &camera_.rotation_.x, 0.01f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("DirectionalLight1"))
	{
		ImGui::Checkbox("IsEnable", &isDirectionalLightEnable_[0]);
		ImGui::ColorEdit3("Color", &directionalLightData_[0].color.x);
		ImGui::DragFloat3("Direction", &directionalLightData_[0].direction.x, 0.01f);
		ImGui::DragFloat("Intensity", &directionalLightData_[0].intensity, 0.01f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("DirectionalLight2"))
	{
		ImGui::Checkbox("IsEnable", &isDirectionalLightEnable_[1]);
		ImGui::ColorEdit3("Color", &directionalLightData_[1].color.x);
		ImGui::DragFloat3("Direction", &directionalLightData_[1].direction.x, 0.01f);
		ImGui::DragFloat("Intensity", &directionalLightData_[1].intensity, 0.01f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("PointLight1"))
	{
		ImGui::Checkbox("IsEnable", &isPointLightEnable_[0]);
		ImGui::ColorEdit3("Color", &pointLightData_[0].color.x);
		ImGui::DragFloat3("Position", &pointLightData_[0].position.x, 0.01f);
		ImGui::DragFloat("Intensity", &pointLightData_[0].intensity, 0.01f);
		ImGui::DragFloat("Radius", &pointLightData_[0].radius, 0.1f);
		ImGui::DragFloat("Decay", &pointLightData_[0].decay, 0.01f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("PointLight2"))
	{
		ImGui::Checkbox("IsEnable", &isPointLightEnable_[1]);
		ImGui::ColorEdit3("Color", &pointLightData_[1].color.x);
		ImGui::DragFloat3("Position", &pointLightData_[1].position.x, 0.01f);
		ImGui::DragFloat("Intensity", &pointLightData_[1].intensity, 0.01f);
		ImGui::DragFloat("Radius", &pointLightData_[1].radius, 0.1f);
		ImGui::DragFloat("Decay", &pointLightData_[1].decay, 0.01f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("SpotLight1"))
	{
		ImGui::Checkbox("IsEnable", &isSpotLightEnable_[0]);
		ImGui::ColorEdit3("Color", &spotLightData_[0].color.x);
		ImGui::DragFloat3("Position", &spotLightData_[0].position.x, 0.01f);
		ImGui::DragFloat("Intensity", &spotLightData_[0].intensity, 0.01f);
		ImGui::DragFloat3("Direction", &spotLightData_[0].direction.x, 0.01f);
		ImGui::DragFloat("Distance", &spotLightData_[0].distance, 0.1f);
		ImGui::DragFloat("Decay", &spotLightData_[0].decay, 0.01f);
		ImGui::DragFloat("CosFalloffStart", &spotLightData_[0].cosFalloffStart, 0.01f);
		ImGui::DragFloat("CosAngle", &spotLightData_[0].cosAngle, 0.01f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("SpotLight2"))
	{
		ImGui::Checkbox("IsEnable", &isSpotLightEnable_[1]);
		ImGui::ColorEdit3("Color", &spotLightData_[1].color.x);
		ImGui::DragFloat3("Position", &spotLightData_[1].position.x, 0.01f);
		ImGui::DragFloat("Intensity", &spotLightData_[1].intensity, 0.01f);
		ImGui::DragFloat3("Direction", &spotLightData_[1].direction.x, 0.01f);
		ImGui::DragFloat("Distance", &spotLightData_[1].distance, 0.1f);
		ImGui::DragFloat("Decay", &spotLightData_[1].decay, 0.01f);
		ImGui::DragFloat("CosFalloffStart", &spotLightData_[1].cosFalloffStart, 0.01f);
		ImGui::DragFloat("CosAngle", &spotLightData_[1].cosAngle, 0.01f);
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
	//地形の描画
	terrainModel_->Draw(terrainWorldTransform_, camera_);

	//モンスターボールの描画
	monsterBallModel_->Draw(monsterBallWorldTransform_, camera_);

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

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}