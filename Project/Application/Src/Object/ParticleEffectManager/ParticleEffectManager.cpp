#include "ParticleEffectManager.h"

void ParticleEffectManager::Initialize(const Camera* camera)
{
	//パーティクルマネージャーの初期化
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Clear();
	particleManager_->SetCamera(camera);

	//テクスチャの読み込み
	TextureManager::Load("smoke_01.png");

	//パーティクルシステムの作成
	particleSystems_["Normal"] = ParticleManager::Create("Normal");
	particleSystems_["Smoke"] = ParticleManager::Create("Smoke");
	particleSystems_["Smoke"]->SetTexture("smoke_01.png");

	//新しく追加するパーティクルエフェクトの設定名のバッファのサイズを設定
	newParticleEffectSettingsName_.resize(128);

	//新しく追加するパーティクルシステムの設定名のバッファのサイズを設定
	newParticleSystemSettingsName_.resize(128);
}

void ParticleEffectManager::Update()
{
	//ImGuiウィンドウの開始
	ImGui::Begin("ParticleEffectManager");

	//新しいパーティクルエフェクトの名前を入力
	ImGui::InputText("ParticleEffectName", newParticleEffectSettingsName_.data(), newParticleEffectSettingsName_.size());
	//パーティクルエフェクトを追加
	if (ImGui::Button("AddParticlelEffect"))
	{
		//名前が入力されている場合は追加する
		if (newParticleEffectSettingsName_.find_first_not_of('\0') != std::string::npos)
		{
			//同じ名前のエフェクトを探す
			auto it = particleEffectSettings_.find(newParticleEffectSettingsName_);
			//同じエフェクトが見つからなかった場合
			if (it == particleEffectSettings_.end())
			{
				//新しいエフェクトを追加
				particleEffectSettings_[newParticleEffectSettingsName_] = ParticleEffectSettings();
				//名前のバッファをクリアしてリサイズする
				newParticleEffectSettingsName_.clear();
				newParticleEffectSettingsName_.resize(128);
			}
		}
	}

	//パーティクルエフェクトの名前をまとめる
	std::vector<std::string> particleEffectNames{};
	for (const auto& particleEffectSetting : particleEffectSettings_)
	{
		particleEffectNames.push_back(particleEffectSetting.first);
	}

	//現在のアイテムの名前を設定
	std::string currentParticleEffectName = currentParticleEffectSettingsIndex_ < particleEffectNames.size() ? particleEffectNames[currentParticleEffectSettingsIndex_] : "None";
	//コンボボックスの作成
	if (ImGui::BeginCombo("ParticleEffectSetting", currentParticleEffectName.data()))
	{
		//各アイテムを表示
		for (size_t i = 0; i < particleEffectNames.size(); ++i)
		{
			if (ImGui::Selectable(particleEffectNames[i].c_str(), currentParticleEffectSettingsIndex_ == i)) 
			{
				//選択されたインデックスを更新
				currentParticleEffectSettingsIndex_ = static_cast<int>(i); 
			}
		}
		//コンボボックスの終了
		ImGui::EndCombo();
	}

	//現在の選択されているパーティクルエフェクトがある場合は早期リターン
	if (currentParticleEffectName == "None")
	{
		ImGui::End();
		return;
	}

	//区切り線を入れる
	ImGui::SeparatorText(currentParticleEffectName.data());

	//現在のパーティクルエフェクトの設定を取得
	ParticleEffectSettings& selectParticleEffectSetting = particleEffectSettings_[currentParticleEffectName];

	//新しいパーティクルシステムの設定名を入力
	ImGui::InputText("ParticleSystemSettingsName", newParticleSystemSettingsName_.data(), newParticleSystemSettingsName_.size());
	//パーティクルエフェクトを追加
	if (ImGui::Button("AddParticleSystemSettings"))
	{
		//名前が入力されている場合は追加する
		if (newParticleSystemSettingsName_.find_first_not_of('\0') != std::string::npos)
		{
			//同じ名前のエフェクトを探す
			auto it = selectParticleEffectSetting.particleSystemSetting.find(newParticleSystemSettingsName_);
			//同じエフェクトが見つからなかった場合
			if (it == selectParticleEffectSetting.particleSystemSetting.end())
			{
				//新しいエフェクトを追加
				selectParticleEffectSetting.particleSystemSetting[newParticleSystemSettingsName_] = ParticleSystemSettings();
				//名前のバッファをクリアしてリサイズする
				newParticleSystemSettingsName_.clear();
				newParticleSystemSettingsName_.resize(128);
			}
		}
	}

	//全てのパーティクルエフェクトの設定について編集
	for (auto& particleEffectSetting : selectParticleEffectSetting.particleSystemSetting)
	{
		//パーティクルシステムの設定を編集
		if (ImGui::TreeNode(particleEffectSetting.first.data()))
		{
			//現在のパーティクルシステムの設定を取得
			ParticleSystemSettings& particleSystemSettings = particleEffectSetting.second;

			//パーティクルエミッターの設定を編集
			if (ImGui::CollapsingHeader("ParticleEmitterSettings"))
			{
				//エミッターの設定を取得
				ParticleEmitterSettings& particleEmitterSettings = particleSystemSettings.particleEmitterSettings;

				//各パラメーターを設定
				ImGui::DragFloat("EmitterLifeTime", &particleEmitterSettings.emitterLifeTime, 0.01f);
				ImGui::DragFloat3("Translation", &particleEmitterSettings.translate.x, 0.01f);
				ImGui::DragFloat("Radius", &particleEmitterSettings.radius, 0.01f);
				ImGui::DragInt("Count", &particleEmitterSettings.count);
				ImGui::DragFloat3("RotateMin", &particleEmitterSettings.rotateMin.x, 0.01f);
				ImGui::DragFloat3("RotateMax", &particleEmitterSettings.rotateMax.x, 0.01f);
				ImGui::DragFloat3("ScaleMin", &particleEmitterSettings.scaleMin.x, 0.01f);
				ImGui::DragFloat3("ScaleMax", &particleEmitterSettings.scaleMax.x, 0.01f);
				ImGui::DragFloat3("VelocityMin", &particleEmitterSettings.velocityMin.x, 0.01f);
				ImGui::DragFloat3("VelocityMax", &particleEmitterSettings.velocityMax.x, 0.01f);
				ImGui::DragFloat("LifeTimeMin", &particleEmitterSettings.lifeTimeMin, 0.01f);
				ImGui::DragFloat("LifeTimeMax", &particleEmitterSettings.lifeTimeMax, 0.01f);
				ImGui::ColorEdit4("ColorMin", &particleEmitterSettings.colorMin.x);
				ImGui::ColorEdit4("ColorMax", &particleEmitterSettings.colorMax.x);
				ImGui::DragFloat("Frequency", &particleEmitterSettings.frequency, 0.01f);
				ImGui::Checkbox("AlignToDirection", &particleEmitterSettings.alignToDirection);
				ImGui::Checkbox("EnableColorOverLifeTime", &particleEmitterSettings.enableColorOverLifeTime);
				ImGui::ColorEdit3("ColorMin", &particleEmitterSettings.targetColor.x);
				ImGui::Checkbox("EnableAlphaOverLifeTime", &particleEmitterSettings.enableAlphaOverLifeTime);
				ImGui::DragFloat("TargetAlpha", &particleEmitterSettings.targetAlpha, 0.01f, 0.0f, 1.0f);
				ImGui::Checkbox("EnableSizeOverLifeTime", &particleEmitterSettings.enableSizeOverLifeTime);
				ImGui::DragFloat3("TargetScale", &particleEmitterSettings.targetScale.x, 0.01f);
				ImGui::Checkbox("EnableRotationOverLifeTime", &particleEmitterSettings.enableRotationOverLifeTime);
				ImGui::DragFloat3("RotSpeed", &particleEmitterSettings.rotSpeed.x, 0.01f);
			}

			//加速フィールドの設定を編集
			if (ImGui::CollapsingHeader("ParticleAccelerationFieldSettings"))
			{
				//加速フィールドの設定を取得
				ParticleAccelerationFieldSettings& particleAccelerationFieldSettings = particleSystemSettings.particleAccelerationFieldSettings;

				//各パラメーターを設定
				ImGui::DragFloat("LifeTime", &particleAccelerationFieldSettings.deathTimer_, 0.01f);
				ImGui::DragFloat3("Acceleration", &particleAccelerationFieldSettings.acceleration_.x, 0.01f);
				ImGui::DragFloat3("Translation", &particleAccelerationFieldSettings.translate_.x, 0.01f);
				ImGui::DragFloat3("Min", &particleAccelerationFieldSettings.min_.x, 0.01f);
				ImGui::DragFloat3("Max", &particleAccelerationFieldSettings.max_.x, 0.01f);
			}

			//加速フィールドの設定を編集
			if (ImGui::CollapsingHeader("ParticleGravityFieldSettings"))
			{
				//加速フィールドの設定を取得
				ParticleGravityFieldSettings& particleGravityFieldSettings = particleSystemSettings.particleGravityFieldSettings;

				//各パラメーターを設定
				ImGui::DragFloat("LifeTime", &particleGravityFieldSettings.deathTimer, 0.01f);
				ImGui::DragFloat("Strength", &particleGravityFieldSettings.strength, 0.01f);
				ImGui::DragFloat("StopDistance", &particleGravityFieldSettings.stopDistance, 0.01f);
				ImGui::DragFloat3("Translation", &particleGravityFieldSettings.translate.x, 0.01f);
				ImGui::DragFloat3("Min", &particleGravityFieldSettings.min.x, 0.01f);
				ImGui::DragFloat3("Max", &particleGravityFieldSettings.max.x, 0.01f);
			}

			ImGui::TreePop();
		}
	}

	//ImGuiウィンドウの終了
	ImGui::End();
}