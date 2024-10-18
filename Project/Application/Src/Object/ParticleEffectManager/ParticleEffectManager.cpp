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

	//新しく追加するパーティクルエミッターの設定名のバッファのサイズを設定
	newParticleEmitterSettingsName_.resize(128);

	//新しく追加する加速フィールドの設定名のバッファのサイズを設定
	newParticleAccelerationFieldSettingsName_.resize(128);

	//新しく追加する重力の設定名のバッファのサイズを設定
	newParticleGravityFieldSettingsName_.resize(128);
}

void ParticleEffectManager::Update()
{
	//ImGuiウィンドウの開始
	ImGui::Begin("ParticleEffectManager");

	//新しいパーティクルエフェクトを追加
	AddNewParticleEffect();

	//パーティクルエフェクトの名前をまとめる
	std::vector<std::string> particleEffectNames = GetParticleEffectNames();

	//コンボボックスでエフェクトを選択
	SelectParticleEffect(particleEffectNames);

	//選択されているパーティクルエフェクトがない場合は終了
	if (currentParticleEffectSettingsIndex_ >= particleEffectNames.size())
	{
		ImGui::End();
		return;
	}

	//現在のパーティクルエフェクトの名前を取得
	const std::string& currentParticleEffectName = particleEffectNames[currentParticleEffectSettingsIndex_];

	//現在のパーティクルエフェクトの設定を取得
	ParticleEffectSettings& selectedParticleEffectSetting = particleEffectSettings_[currentParticleEffectName];

	//区切り線を入れる
	ImGui::SeparatorText(currentParticleEffectName.c_str());

	//新しいパーティクルシステムの設定を追加
	AddNewParticleSystemSettings(selectedParticleEffectSetting);

	//パーティクルシステムの設定を編集
	EditParticleSystemSettings(selectedParticleEffectSetting);

	//ImGuiウィンドウの終了
	ImGui::End();
}

void ParticleEffectManager::AddNewParticleEffect()
{
	//新しいパーティクルエフェクトの名前を入力
	ImGui::InputText("ParticleEffectName", newParticleEffectSettingsName_.data(), newParticleEffectSettingsName_.size());
	//パーティクルエフェクトを追加
	if (ImGui::Button("AddParticlelEffect"))
	{
		//名前が入力されていない場合は処理を飛ばす
		if (newParticleEffectSettingsName_.find_first_not_of('\0') == std::string::npos)
		{
			return;
		}

		//同じエフェクトが見つからなかった場合
		if (particleEffectSettings_.find(newParticleEffectSettingsName_) == particleEffectSettings_.end())
		{
			//新しいエフェクトを追加
			particleEffectSettings_[newParticleEffectSettingsName_] = ParticleEffectSettings();
			//名前のバッファをクリアしてリサイズする
			ClearAndResizeBuffer(newParticleEffectSettingsName_);
		}
	}
}

void ParticleEffectManager::ClearAndResizeBuffer(std::string& buffer)
{
	//名前をクリア
	buffer.clear();
	//サイズを再設定
	buffer.resize(128);
}

const std::vector<std::string> ParticleEffectManager::GetParticleEffectNames() const
{
	std::vector<std::string> names;
	for (const auto& effect : particleEffectSettings_)
	{
		names.push_back(effect.first);
	}
	return names;
}

void ParticleEffectManager::SelectParticleEffect(const std::vector<std::string>& particleEffectNames)
{
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
}

void ParticleEffectManager::AddNewParticleSystemSettings(ParticleEffectSettings& effectSettings)
{
	//新しいパーティクルシステムの設定名を入力
	ImGui::InputText("ParticleSystemSettingsName", newParticleSystemSettingsName_.data(), newParticleSystemSettingsName_.size());
	//パーティクルシステムの設定を追加
	if (ImGui::Button("AddParticleSystemSettings"))
	{
		//名前が入力されていない場合は処理を飛ばす
		if (newParticleSystemSettingsName_.find_first_not_of('\0') == std::string::npos)
		{
			return;
		}

		//同じパーティクルエミッターの設定が存在しなければ
		if (effectSettings.particleSystemSetting.find(newParticleSystemSettingsName_) == effectSettings.particleSystemSetting.end())
		{
			//新しいエフェクトを追加
			effectSettings.particleSystemSetting[newParticleSystemSettingsName_] = ParticleSystemSettings();
			//名前のバッファをクリアしてリサイズする
			ClearAndResizeBuffer(newParticleEmitterSettingsName_);
		}
	}
}

void ParticleEffectManager::EditParticleSystemSettings(ParticleEffectSettings& effectSettings)
{
	//全てのパーティクルシステムの設定について編集
	for (auto& particleSystemSetting : effectSettings.particleSystemSetting)
	{
		//TreeNodeを展開
		if (ImGui::TreeNode(particleSystemSetting.first.c_str()))
		{
			//区切り線を入れる
			ImGui::SeparatorText("Emitter");

			//パーティクルシステムの設定を取得
			ParticleSystemSettings& systemSettings = particleSystemSetting.second;

			//パーティクルエミッターの設定を編集
			if (ImGui::TreeNode("ParticleEmitterSettings"))
			{
				//新しいパーティクルエミッターの設定を追加
				AddNewParticleEmitterSettings(systemSettings);

				//パーティクルエミッターの設定を編集
				EditParticleEmitterSettings(systemSettings);

				//TreeNodeを閉じる
				ImGui::TreePop();
			}

			//区切り線を入れる
			ImGui::NewLine();
			ImGui::SeparatorText("AccelerationField");

			//加速フィールドの設定を編集
			if (ImGui::TreeNode("ParticleAccelerationFieldSettings"))
			{
				//新しい加速フィールドの設定を追加
				AddNewParticleAccelerationFieldSettings(systemSettings);

				//加速フィールドの設定を編集
				EditParticleAccelerationFieldSettings(systemSettings);

				//TreeNodeを閉じる
				ImGui::TreePop();
			}

			//区切り線を入れる
			ImGui::NewLine();
			ImGui::SeparatorText("GravityField");

			//重力フィールドの設定を編集
			if (ImGui::TreeNode("ParticleGravityFieldSettings"))
			{
				//新しい重力フィールドの設定を追加
				AddNewParticleGravityFieldSettings(systemSettings);

				//重力フィールドの設定を編集
				EditParticleGravityFieldSettings(systemSettings);

				//TreeNodeを閉じる
				ImGui::TreePop();
			}

			//TreeNodeを閉じる
			ImGui::TreePop();
		}
	}
}

void ParticleEffectManager::AddNewParticleEmitterSettings(ParticleSystemSettings& systemSettings)
{
	//新しいパーティクルエミッターの設定名を入力
	ImGui::InputText("ParticleEmitterSettingsName", newParticleEmitterSettingsName_.data(), newParticleEmitterSettingsName_.size());

	//パーティクルシステムの設定を追加
	if (ImGui::Button("AddParticleEmitterSettings"))
	{
		//名前が入力されていない場合は処理を飛ばす
		if (newParticleEmitterSettingsName_.find_first_not_of('\0') == std::string::npos)
		{
			return;
		}

		//同じパーティクルエミッターの設定が存在しなければ
		if (systemSettings.particleEmitterSettings.find(newParticleEmitterSettingsName_) == systemSettings.particleEmitterSettings.end())
		{
			//新しいパーティクルエミッターを追加
			systemSettings.particleEmitterSettings[newParticleEmitterSettingsName_] = ParticleEmitterSettings();
			//名前のバッファをクリアしてリサイズする
			ClearAndResizeBuffer(newParticleEmitterSettingsName_);
		}
	}
}

void ParticleEffectManager::EditParticleEmitterSettings(ParticleSystemSettings& systemSettings)
{
	//エミッターのカウンター
	uint32_t counter = 0;

	//全てのパーティクルエミッターの設定を編集
	for (auto& particleEmitterSetting : systemSettings.particleEmitterSettings)
	{
		//TreeNodeの名前を設定
		std::string treeNodeName = "ParticleEmitter" + std::to_string(counter);

		//パーティクルエミッターのTreeNodeを展開
		if (ImGui::TreeNode(treeNodeName.c_str(), particleEmitterSetting.first.c_str()))
		{
			//パーティクルエミッターの設定を取得
			ParticleEmitterSettings& emitterSettings = particleEmitterSetting.second;

			//各パラメーターを編集
			ImGui::DragFloat("EmitterLifeTime", &emitterSettings.emitterLifeTime, 0.01f);
			ImGui::DragFloat3("Translation", &emitterSettings.translate.x, 0.01f);
			ImGui::DragFloat("Radius", &emitterSettings.radius, 0.01f);
			ImGui::DragInt("Count", &emitterSettings.count);
			ImGui::DragFloat3("RotateMin", &emitterSettings.rotateMin.x, 0.01f);
			ImGui::DragFloat3("RotateMax", &emitterSettings.rotateMax.x, 0.01f);
			ImGui::DragFloat3("ScaleMin", &emitterSettings.scaleMin.x, 0.01f);
			ImGui::DragFloat3("ScaleMax", &emitterSettings.scaleMax.x, 0.01f);
			ImGui::DragFloat3("VelocityMin", &emitterSettings.velocityMin.x, 0.01f);
			ImGui::DragFloat3("VelocityMax", &emitterSettings.velocityMax.x, 0.01f);
			ImGui::DragFloat("LifeTimeMin", &emitterSettings.lifeTimeMin, 0.01f);
			ImGui::DragFloat("LifeTimeMax", &emitterSettings.lifeTimeMax, 0.01f);
			ImGui::ColorEdit4("ColorMin", &emitterSettings.colorMin.x);
			ImGui::ColorEdit4("ColorMax", &emitterSettings.colorMax.x);
			ImGui::DragFloat("Frequency", &emitterSettings.frequency, 0.01f);
			ImGui::Checkbox("AlignToDirection", &emitterSettings.alignToDirection);
			ImGui::Checkbox("EnableColorOverLifeTime", &emitterSettings.enableColorOverLifeTime);
			ImGui::ColorEdit3("TargetColor", &emitterSettings.targetColor.x);
			ImGui::Checkbox("EnableAlphaOverLifeTime", &emitterSettings.enableAlphaOverLifeTime);
			ImGui::DragFloat("TargetAlpha", &emitterSettings.targetAlpha, 0.01f, 0.0f, 1.0f);
			ImGui::Checkbox("EnableSizeOverLifeTime", &emitterSettings.enableSizeOverLifeTime);
			ImGui::DragFloat3("TargetScale", &emitterSettings.targetScale.x, 0.01f);
			ImGui::Checkbox("EnableRotationOverLifeTime", &emitterSettings.enableRotationOverLifeTime);
			ImGui::DragFloat3("RotSpeed", &emitterSettings.rotSpeed.x, 0.01f);

			//TreeNodeを閉じる
			ImGui::TreePop();
		}

		//カウンターを増やす
		counter++;
	}
}

void ParticleEffectManager::AddNewParticleAccelerationFieldSettings(ParticleSystemSettings& systemSettings)
{
	//新しい加速フィールドの設定名を入力
	ImGui::InputText("ParticleAccelerationFieldSettingsName", newParticleAccelerationFieldSettingsName_.data(), newParticleAccelerationFieldSettingsName_.size());

	//パーティクルシステムの設定を追加
	if (ImGui::Button("AddParticleAccelerationFieldSettings"))
	{
		//名前が入力されていない場合は処理を飛ばす
		if (newParticleAccelerationFieldSettingsName_.find_first_not_of('\0') == std::string::npos)
		{
			return;
		}

		//同じ加速フィールドの設定が存在しなければ
		if (systemSettings.particleAccelerationFieldSettings.find(newParticleAccelerationFieldSettingsName_) == systemSettings.particleAccelerationFieldSettings.end())
		{
			//新しい加速フィールドを追加
			systemSettings.particleAccelerationFieldSettings[newParticleAccelerationFieldSettingsName_] = ParticleAccelerationFieldSettings();
			//名前のバッファをクリアしてリサイズする
			ClearAndResizeBuffer(newParticleAccelerationFieldSettingsName_);
		}
	}
}

void ParticleEffectManager::EditParticleAccelerationFieldSettings(ParticleSystemSettings& systemSettings)
{
	//加速フィールドのカウンター
	uint32_t counter = 0;

	//全ての加速フィールドの設定を編集
	for (auto& particleAccelerationFieldSettings : systemSettings.particleAccelerationFieldSettings)
	{
		//TreeNodeの名前を設定
		std::string treeNodeName = "ParticleAccelerationField" + std::to_string(counter);

		//加速フィールドのTreeNodeを展開
		if (ImGui::TreeNode(treeNodeName.c_str(), particleAccelerationFieldSettings.first.c_str()))
		{
			//加速フィールドの設定を取得
			ParticleAccelerationFieldSettings& particleAccelerationFieldSetting = particleAccelerationFieldSettings.second;

			//各パラメーターを編集
			ImGui::DragFloat("LifeTime", &particleAccelerationFieldSetting.deathTimer_, 0.01f);
			ImGui::DragFloat3("Translation", &particleAccelerationFieldSetting.translate_.x, 0.01f);
			ImGui::DragFloat3("Min", &particleAccelerationFieldSetting.min_.x, 0.01f);
			ImGui::DragFloat3("Max", &particleAccelerationFieldSetting.max_.x, 0.01f);
			ImGui::DragFloat3("Acceleration", &particleAccelerationFieldSetting.acceleration_.x, 0.01f);

			//TreeNodeを閉じる
			ImGui::TreePop();
		}

		//カウンターを増やす
		counter++;
	}
}

void ParticleEffectManager::AddNewParticleGravityFieldSettings(ParticleSystemSettings& systemSettings)
{
	//新しい重力フィールドの設定名を入力
	ImGui::InputText("ParticleGravityFieldSettingsName", newParticleGravityFieldSettingsName_.data(), newParticleGravityFieldSettingsName_.size());

	//重力フィールドの設定を追加
	if (ImGui::Button("AddParticleGravityFieldSettings"))
	{
		//名前が入力されていない場合は処理を飛ばす
		if (newParticleGravityFieldSettingsName_.find_first_not_of('\0') == std::string::npos)
		{
			return;
		}

		//同じ重力フィールドの設定が存在しなければ
		if (systemSettings.particleGravityFieldSettings.find(newParticleGravityFieldSettingsName_) == systemSettings.particleGravityFieldSettings.end())
		{
			//新しい重力フィールドを追加
			systemSettings.particleGravityFieldSettings[newParticleGravityFieldSettingsName_] = ParticleGravityFieldSettings();
			//名前のバッファをクリアしてリサイズする
			ClearAndResizeBuffer(newParticleGravityFieldSettingsName_);
		}
	}
}

void ParticleEffectManager::EditParticleGravityFieldSettings(ParticleSystemSettings& systemSettings)
{
	//重力フィールドのカウンター
	uint32_t counter = 0;

	//全ての重力フィールドの設定を編集
	for (auto& particleGravityFieldSettings : systemSettings.particleGravityFieldSettings)
	{
		//TreeNodeの名前を設定
		std::string treeNodeName = "ParticleGravityField" + std::to_string(counter);

		//重力フィールドのTreeNodeを展開
		if (ImGui::TreeNode(treeNodeName.c_str(), particleGravityFieldSettings.first.c_str()))
		{
			//重力フィールドの設定を取得
			ParticleGravityFieldSettings& particleGravityFieldSetting = particleGravityFieldSettings.second;

			//各パラメーターを編集
			ImGui::DragFloat("LifeTime", &particleGravityFieldSetting.deathTimer, 0.01f);
			ImGui::DragFloat3("Translation", &particleGravityFieldSetting.translate.x, 0.01f);
			ImGui::DragFloat3("Min", &particleGravityFieldSetting.min.x, 0.01f);
			ImGui::DragFloat3("Max", &particleGravityFieldSetting.max.x, 0.01f);
			ImGui::DragFloat("Strength", &particleGravityFieldSetting.strength, 0.01f);
			ImGui::DragFloat("StopDistance", &particleGravityFieldSetting.stopDistance, 0.01f);

			//TreeNodeを閉じる
			ImGui::TreePop();
		}

		//カウンターを増やす
		counter++;
	}
}