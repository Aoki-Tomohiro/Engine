#include "ParticleEffectManager.h"

void ParticleEffectManager::Initialize(const Camera* camera)
{
	//パーティクルマネージャーの初期化
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Clear();
	particleManager_->SetCamera(camera);

	//テクスチャの読み込み
	TextureManager::Load("smoke_01.png");
	TextureManager::Load("smoke_10.png");
	TextureManager::Load("star_07.png");
	TextureManager::Load("ShockWave.png");
	TextureManager::Load("Flare.png");
	TextureManager::Load("twirl_03.png");
	TextureManager::Load("muzzle_01.png");
	TextureManager::Load("Crack.png");

	//デフォルトのパーティクルの作成
	particleSystems_["Normal"] = ParticleManager::Create("Normal");
	//煙のパーティクルを作成
	particleSystems_["Smoke"] = ParticleManager::Create("Smoke");
	particleSystems_["Smoke"]->SetTexture("smoke_01.png");
	//煙の輪パーティクルを作成
	particleSystems_["SmokeRing"] = ParticleManager::Create("SmokeRing");
	particleSystems_["SmokeRing"]->SetTexture("smoke_10.png");
	//光のパーティクルを作成
	particleSystems_["Light"] = ParticleManager::Create("Light");
	particleSystems_["Light"]->SetTexture("star_07.png");
	//円のパーティクルを作成
	particleSystems_["Circle"] = ParticleManager::Create("Circle");
	particleSystems_["Circle"]->SetTexture("ShockWave.png");
	//光線のパーティクルを作成
	particleSystems_["Flare"] = ParticleManager::Create("Flare");
	particleSystems_["Flare"]->SetTexture("Flare.png");
	//渦巻きのパーティクルを作成
	particleSystems_["Twirl"] = ParticleManager::Create("Twirl");
	particleSystems_["Twirl"]->SetTexture("twirl_03.png");
	//フラッシュのパーティクルを作成
	particleSystems_["Flash"] = ParticleManager::Create("Flash");
	particleSystems_["Flash"]->SetTexture("muzzle_01.png");
	//割れ目のパーティクルを作成
	particleSystems_["Crack"] = ParticleManager::Create("Crack");
	particleSystems_["Crack"]->SetTexture("Crack.png");
	particleSystems_["Crack"]->SetEnableDepthWrite(true);
	//岩のパーティクルを作成
	particleSystems_["Rock"] = ParticleManager::Create("Rock");
	particleSystems_["Rock"]->SetEnableDepthWrite(true);
	particleSystems_["Rock"]->SetBlendMode(BlendMode::kBlendModeNormal);
	particleSystems_["Rock"]->SetModel("Rock");
	//火花のパーティクルを作成
	particleSystems_["Spark"] = ParticleManager::Create("Spark");
	particleSystems_["Spark"]->SetModel("Cube");
	if (Model* sparkModel = particleSystems_["Spark"]->GetModel())
	{
		for (int32_t i = 0; i < sparkModel->GetNumMaterials(); ++i)
		{
			sparkModel->GetMaterial(i)->SetEnableLighting(false);
		}
	}

	//入力用のバッファのサイズを設定
	ResizeNameBuffers(128);

	//設定ファイルを読み込み
	LoadFiles();
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

	//保存
	if (ImGui::Button("Save"))
	{
		SaveFile(currentParticleEffectName);
	}

	//次のアイテムを同じ行に配置
	ImGui::SameLine();

	//読み込み
	if (ImGui::Button("Load"))
	{
		LoadFile(currentParticleEffectName);
	}

	//現在編集中のパーティクルを生成
	if (ImGui::Button("CreateParticles"))
	{
		CreateParticles(currentParticleEffectName, { 0.0f,0.0f,0.0f }, Mathf::IdentityQuaternion());
	}

	//新しいパーティクルシステムの設定を追加
	AddNewParticleSystemSettings(selectedParticleEffectSetting);

	//パーティクルシステムの設定を編集
	EditParticleSystemSettings(selectedParticleEffectSetting);

	//ImGuiウィンドウの終了
	ImGui::End();
}

void ParticleEffectManager::CreateParticles(const std::string& particleEffectName, const Vector3& position, const Quaternion& quaternion)
{
	//パーティクルエフェクトが存在しなければ早期リターン
	if (particleEffectSettings_.find(particleEffectName) == particleEffectSettings_.end())
	{
		return;
	}

	//パーティクルエフェクトの設定を取得
	ParticleEffectSettings particleEffectSettings = particleEffectSettings_[particleEffectName];

	//パーティクルエフェクトの設定をもとにパーティクルを生成
	for (const auto& particleSystemSettings : particleEffectSettings.particleSystemSettings)
	{
		//パーティクルシステムの設定を取得
		const ParticleSystemSettings& particleSystemSetting = particleSystemSettings.second;

		//エミッターの生成
		CreateEmitters(particleSystemSettings.first, particleSystemSetting, position, quaternion);

		//全ての加速フィールドを生成
		for (const auto& accelerationFieldSettings : particleSystemSetting.accelerationFieldSettings)
		{
			//加速フィールドの設定を取得
			const AccelerationFieldSettings& accelerationFieldSetting = accelerationFieldSettings.second;

			//座標を設定
			Vector3 translation = position + Mathf::RotateVector(accelerationFieldSetting.translate, quaternion);

			//加速度を設定
			Vector3 acceleration = Mathf::RotateVector(accelerationFieldSetting.acceleration, quaternion);

			//加速フィールドを生成
			AccelerationField* newAccelerationField = new AccelerationField();
			newAccelerationField->Initialize(accelerationFieldSettings.first, accelerationFieldSetting.deathTimer);
			newAccelerationField->SetTranslate(translation);
			newAccelerationField->SetMin(accelerationFieldSetting.min);
			newAccelerationField->SetMax(accelerationFieldSetting.max);
			newAccelerationField->SetAcceleration(acceleration);
			particleSystems_[particleSystemSettings.first]->AddAccelerationField(newAccelerationField);
		}

		//全ての重力フィールドを生成
		for (const auto& gravityFieldSettings : particleSystemSetting.gravityFieldSettings)
		{
			//重力フィールドの設定を取得
			const GravityFieldSettings& gravityFieldSetting = gravityFieldSettings.second;

			//座標を設定
			Vector3 translation = position + Mathf::RotateVector(gravityFieldSetting.translate, quaternion);

			//重力フィールドを生成
			GravityField* newGravityField = new GravityField();
			newGravityField->Initialize(gravityFieldSettings.first, gravityFieldSetting.deathTimer);
			newGravityField->SetTranslate(translation);
			newGravityField->SetMin(gravityFieldSetting.min);
			newGravityField->SetMax(gravityFieldSetting.max);
			newGravityField->SetStrength(gravityFieldSetting.strength);
			newGravityField->SetStopDistance(gravityFieldSetting.stopDistance);
			particleSystems_[particleSystemSettings.first]->AddGravityField(newGravityField);
		}
	}
}

ParticleEmitter* ParticleEffectManager::GetEmitter(const std::string& particleSystemName, const std::string& emitterName) const
{
	//パーティクルシステムを検索する
	auto it = particleSystems_.find(particleSystemName);
	if (it != particleSystems_.end())
	{
		if (ParticleEmitter* emitter = it->second->GetParticleEmitter(emitterName))
		{
			return emitter;
		}
	}
	return nullptr;
}

AccelerationField* ParticleEffectManager::GetAccelerationField(const std::string& particleSystemName, const std::string& accelerationFieldName) const
{
	//パーティクルシステムを検索する
	auto it = particleSystems_.find(particleSystemName);
	if (it != particleSystems_.end())
	{
		if (AccelerationField* accelerationField = it->second->GetAccelerationField(accelerationFieldName))
		{
			return accelerationField;
		}
	}
	return nullptr;
}

GravityField* ParticleEffectManager::GetGravityField(const std::string& particleSystemName, const std::string& gravityFieldName) const
{
	//パーティクルシステムを検索する
	auto it = particleSystems_.find(particleSystemName);
	if (it != particleSystems_.end())
	{
		if (GravityField* gravityField = it->second->GetGravityField(gravityFieldName))
		{
			return gravityField;
		}
	}
	return nullptr;
}

void ParticleEffectManager::ResizeNameBuffers(const size_t newSize)
{
	newParticleEffectSettingsName_.resize(newSize);
	newEmitterSettingsName_.resize(newSize);
	newAccelerationFieldSettingsName_.resize(newSize);
	newGravityFieldSettingsName_.resize(newSize);
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

		//パーティクルシステムの名前を取得
		std::string particleEffectName = newParticleEffectSettingsName_.substr(0, newParticleEffectSettingsName_.find('\0'));

		//同じエフェクトが見つからなかった場合
		if (particleEffectSettings_.find(particleEffectName) == particleEffectSettings_.end())
		{
			//新しいエフェクトを追加
			particleEffectSettings_[particleEffectName] = ParticleEffectSettings();
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
				//選択しているパーティクルシステムをクリア
				selectedParticleSystem_.clear();
			}
		}
		//コンボボックスの終了
		ImGui::EndCombo();
	}
}

void ParticleEffectManager::AddNewParticleSystemSettings(ParticleEffectSettings& effectSettings)
{
	//パーティクルシステムの名前をまとめる
	std::vector<std::string> particleSystemNames = GetParticleSystemNames();

	//パーティクルシステムを選択
	SelectNewParticleSystem(particleSystemNames);

	//現在のパーティクルシステムの名前を取得
	const std::string& currentParticleSystemName = particleSystemNames[currentParticleSystemSettingsIndex_];

	//パーティクルシステムの設定を追加
	if (ImGui::Button("AddParticleSystemSetting"))
	{
		//同じパーティクルシステムの設定が存在しなければ
		if (effectSettings.particleSystemSettings.find(currentParticleSystemName) == effectSettings.particleSystemSettings.end())
		{
			//新しいエフェクトを追加
			effectSettings.particleSystemSettings[currentParticleSystemName] = ParticleSystemSettings();
		}
	}
}

const std::vector<std::string> ParticleEffectManager::GetParticleSystemNames() const
{
	std::vector<std::string> names;
	for (const auto& system : particleSystems_)
	{
		names.push_back(system.first);
	}
	return names;
}

void ParticleEffectManager::SelectNewParticleSystem(const std::vector<std::string>& particleSystemNames)
{
	//現在のアイテムの名前を設定
	std::string currentParticleSystemName = currentParticleSystemSettingsIndex_ < particleSystemNames.size() ? particleSystemNames[currentParticleSystemSettingsIndex_] : "None";

	//コンボボックスの作成
	if (ImGui::BeginCombo("ParticleSystem", currentParticleSystemName.data()))
	{
		//各アイテムを表示
		for (size_t i = 0; i < particleSystemNames.size(); ++i)
		{
			if (ImGui::Selectable(particleSystemNames[i].c_str(), currentParticleSystemSettingsIndex_ == i))
			{
				//選択されたインデックスを更新
				currentParticleSystemSettingsIndex_ = static_cast<int>(i);
			}
		}
		//コンボボックスの終了
		ImGui::EndCombo();
	}
}

void ParticleEffectManager::EditParticleSystemSettings(ParticleEffectSettings& effectSettings)
{
	//区切り線を入れる
	ImGui::NewLine();
	ImGui::SeparatorText("ParticleSystems");

	//編集するパーティクルシステムを選択
	SelectEditParticleSystem(effectSettings);

	//編集中のパーティクルシステムを削除
	RemoveEditParticleSystem(effectSettings);

	//選択されているパーティクルシステムがない場合は飛ばす
	if (selectedParticleSystem_.empty())
	{
		return;
	}

	//パーティクルシステムの設定を取得
	ParticleSystemSettings& systemSetting = effectSettings.particleSystemSettings[selectedParticleSystem_];

	//区切り線を入れる
	ImGui::NewLine();
	ImGui::SeparatorText("EmitterSetting");

	//新しいパーティクルエミッターの設定を追加
	AddNewParticleEmitterSettings(systemSetting);

	//パーティクルエミッターの設定を編集
	EditParticleEmitterSettings(systemSetting);

	//区切り線を入れる
	ImGui::NewLine();
	ImGui::SeparatorText("AccelerationField");

	//新しい加速フィールドの設定を追加
	AddNewParticleAccelerationFieldSettings(systemSetting);

	//加速フィールドの設定を編集
	EditParticleAccelerationFieldSettings(systemSetting);

	//区切り線を入れる
	ImGui::NewLine();
	ImGui::SeparatorText("GravityField");

	//新しい重力フィールドの設定を追加
	AddNewParticleGravityFieldSettings(systemSetting);

	//重力フィールドの設定を編集
	EditParticleGravityFieldSettings(systemSetting);
}

void ParticleEffectManager::SelectEditParticleSystem(ParticleEffectSettings& effectSettings)
{
	//エフェクトに追加されているパーティクルシステムから選択する
	for (auto& particleSystemSettings : effectSettings.particleSystemSettings)
	{
		//パーティクルシステムの名前を取得
		const std::string& systemName = particleSystemSettings.first;

		//パーティクルシステムを選択
		if (ImGui::Selectable(systemName.c_str(), selectedParticleSystem_ == systemName))
		{
			//システムが選択された場合にその名前を保持
			selectedParticleSystem_ = systemName;
		}
	}
}

void ParticleEffectManager::RemoveEditParticleSystem(ParticleEffectSettings& effectSettings)
{
	//パーティクルシステムの設定を削除
	if (ImGui::Button("RemoveParticleSystemSetting"))
	{
		//パーティクルシステムの設定を削除
		effectSettings.particleSystemSettings.erase(selectedParticleSystem_);
		selectedParticleSystem_.clear();
	}
}

void ParticleEffectManager::SaveFile(const std::string& particleEffectName)
{
	//rootのjsonオブジェクトを作成
	nlohmann::json root = nlohmann::json::object();

	//全てのパーティクルシステムのjsonオブジェクトを作成
	nlohmann::json systemsJson = nlohmann::json::object();

	//全てのパーティクルシステムの設定を保存
	for (const auto& particleSystemSettings : particleEffectSettings_[particleEffectName].particleSystemSettings)
	{
		//パーティクルシステムのjsonオブジェクトを作成
		nlohmann::json systemJson = nlohmann::json::object();

		//パーティクルシステムの設定を取得
		const ParticleSystemSettings& particleSystemSetting = particleSystemSettings.second;

		//全てのエミッターのjsonオブジェクトを作成
		nlohmann::json emittersJson = nlohmann::json::object();

		//全てのエミッターの設定を保存
		for (const auto& emitterSettings : particleSystemSetting.emitterSettings)
		{
			//エミッターのjsonオブジェクトを作成
			nlohmann::json emitterJson = nlohmann::json::object();
			//エミッターの情報を書き込む
			const EmitterSettings& emitterSetting = emitterSettings.second;
			emitterJson["Translation"] = nlohmann::json::array({ emitterSetting.translate.x, emitterSetting.translate.y, emitterSetting.translate.z });
			emitterJson["Radius"] = emitterSetting.radius;
			emitterJson["Count"] = emitterSetting.count;
			emitterJson["RotateMin"] = nlohmann::json::array({ emitterSetting.rotateMin.x, emitterSetting.rotateMin.y, emitterSetting.rotateMin.z });
			emitterJson["RotateMax"] = nlohmann::json::array({ emitterSetting.rotateMax.x, emitterSetting.rotateMax.y, emitterSetting.rotateMax.z });
			emitterJson["ScaleMin"] = nlohmann::json::array({ emitterSetting.scaleMin.x, emitterSetting.scaleMin.y, emitterSetting.scaleMin.z });
			emitterJson["ScaleMax"] = nlohmann::json::array({ emitterSetting.scaleMax.x, emitterSetting.scaleMax.y, emitterSetting.scaleMax.z });
			emitterJson["VelocityMin"] = nlohmann::json::array({ emitterSetting.velocityMin.x, emitterSetting.velocityMin.y, emitterSetting.velocityMin.z });
			emitterJson["VelocityMax"] = nlohmann::json::array({ emitterSetting.velocityMax.x, emitterSetting.velocityMax.y, emitterSetting.velocityMax.z });
			emitterJson["LifeTimeMin"] = emitterSetting.lifeTimeMin;
			emitterJson["LifeTimeMax"] = emitterSetting.lifeTimeMax;
			emitterJson["ColorMin"] = nlohmann::json::array({ emitterSetting.colorMin.x, emitterSetting.colorMin.y, emitterSetting.colorMin.z, emitterSetting.colorMin.w });
			emitterJson["ColorMax"] = nlohmann::json::array({ emitterSetting.colorMax.x, emitterSetting.colorMax.y, emitterSetting.colorMax.z, emitterSetting.colorMin.w });
			emitterJson["Frequency"] = emitterSetting.frequency;
			emitterJson["EmitterLifeTime"] = emitterSetting.emitterLifeTime;
			emitterJson["AlignToDirection"] = emitterSetting.alignToDirection;
			emitterJson["EnableColorOverLifeTime"] = emitterSetting.enableColorOverLifeTime;
			emitterJson["TargetColor"] = nlohmann::json::array({ emitterSetting.targetColor.x, emitterSetting.targetColor.y, emitterSetting.targetColor.z });
			emitterJson["EnableAlphaOverLifeTime"] = emitterSetting.enableAlphaOverLifeTime;
			emitterJson["TargetAlpha"] = emitterSetting.targetAlpha;
			emitterJson["EnableSizeOverLifeTime"] = emitterSetting.enableSizeOverLifeTime;
			emitterJson["TargetScale"] = nlohmann::json::array({ emitterSetting.targetScale.x, emitterSetting.targetScale.y, emitterSetting.targetScale.z });
			emitterJson["EnableRotationOverLifeTime"] = emitterSetting.enableRotationOverLifeTime;
			emitterJson["RotSpeed"] = nlohmann::json::array({ emitterSetting.rotSpeed.x, emitterSetting.rotSpeed.y, emitterSetting.rotSpeed.z });
			emitterJson["IsBillboard"] = emitterSetting.isBillboard;
			emitterJson["EmitIn360Degrees"] = emitterSetting.emitIn360Degrees;
			emittersJson[emitterSettings.first] = emitterJson;
		}

		//パーティクルシステムのjsonに全てのエミッターのjsonオブジェクトを追加
		systemJson["Emitters"] = emittersJson;

		//全ての加速フィールドのjsonオブジェクトを作成
		nlohmann::json accelerationFieldsJson = nlohmann::json::object();

		//全ての加速フィールドの設定を保存
		for (const auto& accelerationFieldSettings : particleSystemSetting.accelerationFieldSettings)
		{
			//加速フィールドのjsonオブジェクトを作成
			nlohmann::json accelerationFieldJson = nlohmann::json::object();

			//加速フィールドの設定を書き込む
			const AccelerationFieldSettings& accelerationFieldSetting = accelerationFieldSettings.second;
			accelerationFieldJson["LifeTime"] = accelerationFieldSetting.deathTimer;
			accelerationFieldJson["Translation"] = nlohmann::json::array({ accelerationFieldSetting.translate.x, accelerationFieldSetting.translate.y, accelerationFieldSetting.translate.z });
			accelerationFieldJson["Min"] = nlohmann::json::array({ accelerationFieldSetting.min.x, accelerationFieldSetting.min.y, accelerationFieldSetting.min.z });
			accelerationFieldJson["Max"] = nlohmann::json::array({ accelerationFieldSetting.max.x, accelerationFieldSetting.max.y, accelerationFieldSetting.max.z });
			accelerationFieldJson["Acceleration"] = nlohmann::json::array({ accelerationFieldSetting.acceleration.x, accelerationFieldSetting.acceleration.y, accelerationFieldSetting.acceleration.z });
			accelerationFieldsJson[accelerationFieldSettings.first] = accelerationFieldJson;
		}

		//パーティクルシステムのjsonに全ての加速フィールドのjsonオブジェクトを追加
		systemJson["AccelerationFields"] = accelerationFieldsJson;

		//全ての重力フィールドのjsonオブジェクトを作成
		nlohmann::json gravityFieldsJson = nlohmann::json::object();

		//全ての重力フィールドの設定を保存
		for (const auto& gravityFieldSettings : particleSystemSetting.gravityFieldSettings)
		{
			//重力フィールドのjsonオブジェクトを作成
			nlohmann::json gravityFieldJson = nlohmann::json::object();

			//重力フィールドの設定を書き込む
			const GravityFieldSettings& gravityFieldSetting = gravityFieldSettings.second;
			gravityFieldJson["LifeTime"] = gravityFieldSetting.deathTimer;
			gravityFieldJson["Translation"] = nlohmann::json::array({ gravityFieldSetting.translate.x, gravityFieldSetting.translate.y, gravityFieldSetting.translate.z });
			gravityFieldJson["Min"] = nlohmann::json::array({ gravityFieldSetting.min.x, gravityFieldSetting.min.y, gravityFieldSetting.min.z });
			gravityFieldJson["Max"] = nlohmann::json::array({ gravityFieldSetting.max.x, gravityFieldSetting.max.y, gravityFieldSetting.max.z });
			gravityFieldJson["Strength"] = gravityFieldSetting.strength;
			gravityFieldJson["StopDistance"] = gravityFieldSetting.stopDistance;
			gravityFieldsJson[gravityFieldSettings.first] = gravityFieldJson;
		}
		
		//パーティクルシステムのjsonに全ての重力フィールドのjsonオブジェクトを追加
		systemJson["GravityFields"] = gravityFieldsJson;

		//パーティクルシステムのjsonを追加
		systemsJson[particleSystemSettings.first] = systemJson;
	}

	//Rootのjsonオブジェクトに追加
	root[particleEffectName]["ParticleSystems"] = systemsJson;

	//ディレクトリがなければ作成する
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(kDirectoryPath))
	{
		std::filesystem::create_directory(kDirectoryPath);
	}

	//書き込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + particleEffectName + ".json";
	//書き込み用ファイルストリーム
	std::ofstream ofs;
	//ファイルを書き込み用に開く
	ofs.open(filePath);

	//ファイルオープン失敗
	if (ofs.fail())
	{
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "ParticleEffectManager", 0);
		assert(0);
		return;
	}

	//ファイルにjson文字列を書き込む（インデント幅4）
	ofs << std::setw(4) << root << std::endl;
	//ファイルを閉じる
	ofs.close();
}

void ParticleEffectManager::LoadFiles()
{
	//保存先のディレクトリパス
	std::filesystem::path dir(kDirectoryPath);
	//ディレクトリが無ければスキップする
	if (!std::filesystem::exists(kDirectoryPath))
	{
		return;
	}

	//ディレクトリを検索
	std::filesystem::directory_iterator dir_it(kDirectoryPath);
	for (const std::filesystem::directory_entry& entry : dir_it)
	{
		//ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();

		//ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		//.jsonファイル以外はスキップ
		if (extension.compare(".json") != 0)
		{
			continue;
		}

		//ファイルを読み込む
		LoadFile(filePath.stem().string());
	}
}

void ParticleEffectManager::LoadFile(const std::string& particleEffectName)
{
	//読み込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + particleEffectName + ".json";
	//読み込み用のファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込みように開く
	ifs.open(filePath);

	//ファイルオープン失敗
	if (ifs.fail())
	{
		std::string message = "Failed open data file for read.";
		MessageBoxA(nullptr, message.c_str(), "ParticleEffectManager", 0);
		assert(0);
		return;
	}

	//json文字列からjsonのデータ構造に展開
	nlohmann::json root;
	ifs >> root;
	//ファイルを閉じる
	ifs.close();

	//パーティクルエフェクトの設定が存在しなければ飛ばす
	if (!root.contains(particleEffectName))
	{
		return;
	}

	//パーティクルエフェクトのデータを取得
	nlohmann::json particleEffectJson = root[particleEffectName];

	//パーティクルエフェクトの設定を取得
	ParticleEffectSettings& particleEffectSettings = particleEffectSettings_[particleEffectName];

	//各パーティクルシステムについて
	for (nlohmann::json::iterator systemItem = particleEffectJson["ParticleSystems"].begin(); systemItem != particleEffectJson["ParticleSystems"].end(); ++systemItem)
	{
		//パーティクルシステムの名前を取得
		const std::string& systemName = systemItem.key();
		//パーティクルシステムのデータを取得
		nlohmann::json systemData = systemItem.value();

		//パーティクルシステムの設定を取得
		ParticleSystemSettings& particleSystemSettings = particleEffectSettings.particleSystemSettings[systemName];

		//エミッターの設定を読み込む
		SetEmitterSettings(particleSystemSettings, systemData);

		//加速フィールドの設定を読み込む
		SetAccelerationFieldSettings(particleSystemSettings, systemData);

		//重力フィールドの設定を読み込む
		SetGravityFieldSettings(particleSystemSettings, systemData);
	}
}

void ParticleEffectManager::SetEmitterSettings(ParticleSystemSettings& particleSystemSettings, nlohmann::json& systemData)
{
	//エミッターの項目がない場合は処理を飛ばす
	if (!systemData.contains("Emitters"))
	{
		return;
	}

	//全てのエミッターのデータを取得
	for (nlohmann::json::iterator emitterItem = systemData["Emitters"].begin(); emitterItem != systemData["Emitters"].end(); ++emitterItem)
	{
		//エミッターの名前を取得
		const std::string& emitterName = emitterItem.key();
		//エミッターのデータを取得
		nlohmann::json emitterData = emitterItem.value();
		//エミッターの設定を取得
		EmitterSettings& emitterSettings = particleSystemSettings.emitterSettings[emitterName];
		emitterSettings.translate = { emitterData["Translation"][0].get<float>(), emitterData["Translation"][1].get<float>(), emitterData["Translation"][2].get<float>() };
		emitterSettings.radius = emitterData["Radius"].get<float>();
		emitterSettings.count = emitterData["Count"].get<int32_t>();
		emitterSettings.rotateMin = { emitterData["RotateMin"][0].get<float>(), emitterData["RotateMin"][1].get<float>(), emitterData["RotateMin"][2].get<float>() };
		emitterSettings.rotateMax = { emitterData["RotateMax"][0].get<float>(), emitterData["RotateMax"][1].get<float>(), emitterData["RotateMax"][2].get<float>() };
		emitterSettings.scaleMin = { emitterData["ScaleMin"][0].get<float>(), emitterData["ScaleMin"][1].get<float>(), emitterData["ScaleMin"][2].get<float>() };
		emitterSettings.scaleMax = { emitterData["ScaleMax"][0].get<float>(), emitterData["ScaleMax"][1].get<float>(), emitterData["ScaleMax"][2].get<float>() };
		emitterSettings.velocityMin = { emitterData["VelocityMin"][0].get<float>(), emitterData["VelocityMin"][1].get<float>(), emitterData["VelocityMin"][2].get<float>() };
		emitterSettings.velocityMax = { emitterData["VelocityMax"][0].get<float>(), emitterData["VelocityMax"][1].get<float>(), emitterData["VelocityMax"][2].get<float>() };
		emitterSettings.lifeTimeMin = emitterData["LifeTimeMin"].get<float>();
		emitterSettings.lifeTimeMax = emitterData["LifeTimeMax"].get<float>();
		emitterSettings.colorMin = { emitterData["ColorMin"][0].get<float>(), emitterData["ColorMin"][1].get<float>(), emitterData["ColorMin"][2].get<float>(), emitterData["ColorMin"][3].get<float>() };
		emitterSettings.colorMax = { emitterData["ColorMax"][0].get<float>(), emitterData["ColorMax"][1].get<float>(), emitterData["ColorMax"][2].get<float>(), emitterData["ColorMax"][3].get<float>() };
		emitterSettings.frequency = emitterData["Frequency"].get<float>();
		emitterSettings.emitterLifeTime = emitterData["EmitterLifeTime"].get<float>();
		emitterSettings.alignToDirection = emitterData["AlignToDirection"].get<bool>();
		emitterSettings.enableColorOverLifeTime = emitterData["EnableColorOverLifeTime"].get<bool>();
		emitterSettings.targetColor = { emitterData["TargetColor"][0].get<float>(), emitterData["TargetColor"][1].get<float>(), emitterData["TargetColor"][2].get<float>() };
		emitterSettings.enableAlphaOverLifeTime = emitterData["EnableAlphaOverLifeTime"].get<bool>();
		emitterSettings.targetAlpha = emitterData["TargetAlpha"].get<float>();
		emitterSettings.enableSizeOverLifeTime = emitterData["EnableSizeOverLifeTime"].get<bool>();
		emitterSettings.targetScale = { emitterData["TargetScale"][0].get<float>(), emitterData["TargetScale"][1].get<float>(), emitterData["TargetScale"][2].get<float>() };
		emitterSettings.enableRotationOverLifeTime = emitterData["EnableRotationOverLifeTime"].get<bool>();
		emitterSettings.rotSpeed = { emitterData["RotSpeed"][0].get<float>(), emitterData["RotSpeed"][1].get<float>(), emitterData["RotSpeed"][2].get<float>() };
		emitterSettings.isBillboard = emitterData["IsBillboard"].get<bool>();
		emitterSettings.emitIn360Degrees = emitterData["EmitIn360Degrees"].get<bool>();
	}
}

void ParticleEffectManager::SetAccelerationFieldSettings(ParticleSystemSettings& particleSystemSettings, nlohmann::json& systemData)
{
	//加速フィールドの項目がない場合は処理を飛ばす
	if (!systemData.contains("AccelerationFields"))
	{
		return;
	}

	//全ての加速フィールドのデータを取得
	for (nlohmann::json::iterator accelerationFieldItem = systemData["AccelerationFields"].begin(); accelerationFieldItem != systemData["AccelerationFields"].end(); ++accelerationFieldItem)
	{
		//加速フィールドの名前を取得
		const std::string& accelerationFieldName = accelerationFieldItem.key();
		//加速フィールドのデータを取得
		nlohmann::json accelerationFieldData = accelerationFieldItem.value();
		//加速フィールドの設定を取得
		AccelerationFieldSettings& accelerationFieldSettings = particleSystemSettings.accelerationFieldSettings[accelerationFieldName];
		accelerationFieldSettings.deathTimer = accelerationFieldData["LifeTime"].get<float>();
		accelerationFieldSettings.translate = { accelerationFieldData["Translation"][0].get<float>(), accelerationFieldData["Translation"][1].get<float>(), accelerationFieldData["Translation"][2].get<float>() };
		accelerationFieldSettings.min = { accelerationFieldData["Min"][0].get<float>(), accelerationFieldData["Min"][1].get<float>(), accelerationFieldData["Min"][2].get<float>() };
		accelerationFieldSettings.max = { accelerationFieldData["Max"][0].get<float>(), accelerationFieldData["Max"][1].get<float>(), accelerationFieldData["Max"][2].get<float>() };
		accelerationFieldSettings.acceleration = { accelerationFieldData["Acceleration"][0].get<float>(), accelerationFieldData["Acceleration"][1].get<float>(), accelerationFieldData["Acceleration"][2].get<float>() };
	}
}

void ParticleEffectManager::SetGravityFieldSettings(ParticleSystemSettings& particleSystemSettings, nlohmann::json& systemData)
{
	//重力フィールドの項目がない場合は処理を飛ばす
	if (!systemData.contains("GravityFields"))
	{
		return;
	}

	//全ての重力フィールドのデータを取得
	for (nlohmann::json::iterator gravityFieldItem = systemData["GravityFields"].begin(); gravityFieldItem != systemData["GravityFields"].end(); ++gravityFieldItem)
	{
		//重力フィールドの名前を取得
		const std::string& gravityFieldName = gravityFieldItem.key();
		//重力フィールドのデータを取得
		nlohmann::json gravityFieldData = gravityFieldItem.value();
		//重力フィールドの設定を取得
		GravityFieldSettings& gravityFieldSettings = particleSystemSettings.gravityFieldSettings[gravityFieldName];
		gravityFieldSettings.deathTimer = gravityFieldData["LifeTime"].get<float>();
		gravityFieldSettings.translate = { gravityFieldData["Translation"][0].get<float>(), gravityFieldData["Translation"][1].get<float>(), gravityFieldData["Translation"][2].get<float>() };
		gravityFieldSettings.min = { gravityFieldData["Min"][0].get<float>(), gravityFieldData["Min"][1].get<float>(), gravityFieldData["Min"][2].get<float>() };
		gravityFieldSettings.max = { gravityFieldData["Max"][0].get<float>(), gravityFieldData["Max"][1].get<float>(), gravityFieldData["Max"][2].get<float>() };
		gravityFieldSettings.strength = gravityFieldData["Strength"].get<float>();
		gravityFieldSettings.stopDistance = gravityFieldData["StopDistance"].get<float>();
	}
}

void ParticleEffectManager::AddNewParticleEmitterSettings(ParticleSystemSettings& systemSettings)
{
	//新しいパーティクルエミッターの設定名を入力
	ImGui::InputText("EmitterName", newEmitterSettingsName_.data(), newEmitterSettingsName_.size());

	//パーティクルシステムの設定を追加
	if (ImGui::Button("AddEmitter"))
	{
		//名前が入力されていない場合は処理を飛ばす
		if (newEmitterSettingsName_.find_first_not_of('\0') == std::string::npos)
		{
			return;
		}

		//エミッターの名前を取得
		std::string emitterName = newEmitterSettingsName_.substr(0, newEmitterSettingsName_.find('\0'));

		//同じパーティクルエミッターの設定が存在しなければ
		if (systemSettings.emitterSettings.find(emitterName) == systemSettings.emitterSettings.end())
		{
			//新しいパーティクルエミッターを追加
			systemSettings.emitterSettings[emitterName] = EmitterSettings();
			//名前のバッファをクリアしてリサイズする
			ClearAndResizeBuffer(newEmitterSettingsName_);
		}
	}
}

void ParticleEffectManager::EditParticleEmitterSettings(ParticleSystemSettings& systemSettings)
{
	//エミッターのカウンター
	uint32_t counter = 0;

	//全てのパーティクルエミッターの設定を編集
	for (auto it = systemSettings.emitterSettings.begin(); it != systemSettings.emitterSettings.end();)
	{
		//TreeNodeの名前を設定
		std::string treeNodeName = "Emitter" + std::to_string(counter);

		//パーティクルエミッターのTreeNodeを展開
		if (ImGui::TreeNode(treeNodeName.c_str(), it->first.c_str()))
		{
			//各パラメーターを編集
			EmitterSettings& emitterSettings = it->second;
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
			ImGui::Checkbox("IsBillboard", &emitterSettings.isBillboard);
			ImGui::Checkbox("EmitIn360Degrees", &emitterSettings.emitIn360Degrees);

			//エミッターの削除
			if (ImGui::Button("RemoveEmitter"))
			{
				//要素を削除しイテレータを更新
				it = systemSettings.emitterSettings.erase(it);
				ImGui::TreePop();
				continue;
			}

			//TreeNodeを閉じる
			ImGui::TreePop();
		}

		//カウンターとイテレータを増やす
		if (it != systemSettings.emitterSettings.end())
		{
			++counter;
			++it;
		}
	}
}

void ParticleEffectManager::AddNewParticleAccelerationFieldSettings(ParticleSystemSettings& systemSettings)
{
	//新しい加速フィールドの設定名を入力
	ImGui::InputText("AccelerationFieldName", newAccelerationFieldSettingsName_.data(), newAccelerationFieldSettingsName_.size());

	//パーティクルシステムの設定を追加
	if (ImGui::Button("AddAccelerationField"))
	{
		//名前が入力されていない場合は処理を飛ばす
		if (newAccelerationFieldSettingsName_.find_first_not_of('\0') == std::string::npos)
		{
			return;
		}

		//加速フィールドの名前を取得
		std::string accelerationFieldName = newAccelerationFieldSettingsName_.substr(0, newAccelerationFieldSettingsName_.find('\0'));

		//同じ加速フィールドの設定が存在しなければ
		if (systemSettings.accelerationFieldSettings.find(accelerationFieldName) == systemSettings.accelerationFieldSettings.end())
		{
			//新しい加速フィールドを追加
			systemSettings.accelerationFieldSettings[accelerationFieldName] = AccelerationFieldSettings();
			//名前のバッファをクリアしてリサイズする
			ClearAndResizeBuffer(newAccelerationFieldSettingsName_);
		}
	}
}

void ParticleEffectManager::EditParticleAccelerationFieldSettings(ParticleSystemSettings& systemSettings)
{
	//加速フィールドのカウンター
	uint32_t counter = 0;

	//全ての加速フィールドの設定を編集
	for (auto it = systemSettings.accelerationFieldSettings.begin(); it != systemSettings.accelerationFieldSettings.end();)
	{
		//TreeNodeの名前を設定
		std::string treeNodeName = "AccelerationField" + std::to_string(counter);

		//加速フィールドのTreeNodeを展開
		if (ImGui::TreeNode(treeNodeName.c_str(), it->first.c_str()))
		{
			//各パラメーターを編集
			AccelerationFieldSettings& particleAccelerationFieldSetting = it->second;
			ImGui::DragFloat("LifeTime", &particleAccelerationFieldSetting.deathTimer, 0.01f);
			ImGui::DragFloat3("Translation", &particleAccelerationFieldSetting.translate.x, 0.01f);
			ImGui::DragFloat3("Min", &particleAccelerationFieldSetting.min.x, 0.01f);
			ImGui::DragFloat3("Max", &particleAccelerationFieldSetting.max.x, 0.01f);
			ImGui::DragFloat3("Acceleration", &particleAccelerationFieldSetting.acceleration.x, 0.01f);

			//加速フィールドを削除
			if (ImGui::Button("RemoveAccelerationField"))
			{
				//要素を削除しイテレータを進める
				it = systemSettings.accelerationFieldSettings.erase(it);
				ImGui::TreePop();
				continue;
			}

			//TreeNodeを閉じる
			ImGui::TreePop();
		}

		//カウンターとイテレータを増やす
		if (it != systemSettings.accelerationFieldSettings.end())
		{
			++counter;
			++it;
		}	
	}
}

void ParticleEffectManager::AddNewParticleGravityFieldSettings(ParticleSystemSettings& systemSettings)
{
	//新しい重力フィールドの設定名を入力
	ImGui::InputText("GravityFieldName", newGravityFieldSettingsName_.data(), newGravityFieldSettingsName_.size());

	//重力フィールドの設定を追加
	if (ImGui::Button("AddGravityField"))
	{
		//名前が入力されていない場合は処理を飛ばす
		if (newGravityFieldSettingsName_.find_first_not_of('\0') == std::string::npos)
		{
			return;
		}

		//重力フィールドの名前を取得
		std::string gravityFieldName = newGravityFieldSettingsName_.substr(0, newGravityFieldSettingsName_.find('\0'));

		//同じ重力フィールドの設定が存在しなければ
		if (systemSettings.gravityFieldSettings.find(gravityFieldName) == systemSettings.gravityFieldSettings.end())
		{
			//新しい重力フィールドを追加
			systemSettings.gravityFieldSettings[gravityFieldName] = GravityFieldSettings();
			//名前のバッファをクリアしてリサイズする
			ClearAndResizeBuffer(newGravityFieldSettingsName_);
		}
	}
}

void ParticleEffectManager::EditParticleGravityFieldSettings(ParticleSystemSettings& systemSettings)
{
	//重力フィールドのカウンター
	uint32_t counter = 0;

	//全ての重力フィールドの設定を編集
	for (auto it = systemSettings.gravityFieldSettings.begin(); it != systemSettings.gravityFieldSettings.end();)
	{
		//TreeNodeの名前を設定
		std::string treeNodeName = "GravityField" + std::to_string(counter);

		//重力フィールドのTreeNodeを展開
		if (ImGui::TreeNode(treeNodeName.c_str(), it->first.c_str()))
		{
			//各パラメーターを編集
			GravityFieldSettings& particleGravityFieldSetting = it->second;
			ImGui::DragFloat("LifeTime", &particleGravityFieldSetting.deathTimer, 0.01f);
			ImGui::DragFloat3("Translation", &particleGravityFieldSetting.translate.x, 0.01f);
			ImGui::DragFloat3("Min", &particleGravityFieldSetting.min.x, 0.01f);
			ImGui::DragFloat3("Max", &particleGravityFieldSetting.max.x, 0.01f);
			ImGui::DragFloat("Strength", &particleGravityFieldSetting.strength, 0.01f);
			ImGui::DragFloat("StopDistance", &particleGravityFieldSetting.stopDistance, 0.01f);

			//重力フィールドを削除
			if (ImGui::Button("RemoveGravityField"))
			{
				it = systemSettings.gravityFieldSettings.erase(it);
				ImGui::TreePop();
				continue;
			}

			//TreeNodeを閉じる
			ImGui::TreePop();
		}

		//カウンターとイテレータを増やす
		if (it != systemSettings.gravityFieldSettings.end())
		{
			++counter;
			++it;
		}
	}
}

void ParticleEffectManager::CreateEmitters(const std::string& particleSystemName, const ParticleSystemSettings& particleSystemSettings, const Vector3& position, const Quaternion& quaternion)
{
	//全てのエミッターを生成
	for (const auto& emitterSettings : particleSystemSettings.emitterSettings)
	{
		//エミッターの設定を取得
		const EmitterSettings& emitterSetting = emitterSettings.second;

		//座標を設定
		Vector3 translation = position + Mathf::RotateVector(emitterSetting.translate, quaternion);

		if (emitterSetting.emitIn360Degrees)
		{
			for (uint32_t i = 0; i < 360; ++i)
			{
				//クォータニオンを計算
				Quaternion rotation = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,0.0f,1.0f }, static_cast<float>(i) * (std::numbers::pi_v<float> / 180.0f));

				//移動方向を計算
				Vector3 direction = Mathf::RotateVector({ 0.0f,1.0f,0.0f }, quaternion * rotation);

				//最小速度を計算
				Vector3 minVelocity = direction * emitterSetting.velocityMin;

				//最大速度を計算
				Vector3 maxVelocity = direction * emitterSetting.velocityMax;

				//エミッターの生成
				ParticleEmitter* newEmitter = EmitterBuilder().SetEmitterName(emitterSettings.first).SetTranslation(translation).SetRadius(emitterSetting.radius).SetCount(emitterSetting.count)
					.SetRotate(emitterSetting.rotateMin, emitterSetting.rotateMax).SetScale(emitterSetting.scaleMin, emitterSetting.scaleMax).SetVelocity(minVelocity, maxVelocity)
					.SetLifeTime(emitterSetting.lifeTimeMin, emitterSetting.lifeTimeMax).SetColor(emitterSetting.colorMin, emitterSetting.colorMax).SetFrequency(emitterSetting.frequency)
					.SetEmitterLifeTime(emitterSetting.emitterLifeTime).SetAlignToDirection(emitterSetting.alignToDirection).SetEnableColorOverLifeTime(emitterSetting.enableColorOverLifeTime)
					.SetTargetColor(emitterSetting.targetColor).SetEnableAlphaOverLifeTime(emitterSetting.enableAlphaOverLifeTime).SetTargetAlpha(emitterSetting.targetAlpha)
					.SetEnableSizeOverLifeTime(emitterSetting.enableSizeOverLifeTime).SetTargetScale(emitterSetting.targetScale).SetEnableRotationOverLifeTime(emitterSetting.enableRotationOverLifeTime)
					.SetRotSpeed(emitterSetting.rotSpeed).SetIsBillboard(emitterSetting.isBillboard).Build();

				//パーティクルシステムにエミッターを追加
				particleSystems_[particleSystemName]->AddParticleEmitter(newEmitter);
			}
		}
		else
		{
			//最小速度を設定
			Vector3 minVelocity = Mathf::RotateVector(emitterSetting.velocityMin, quaternion);

			//最高速度を設定
			Vector3 maxVelocity = Mathf::RotateVector(emitterSetting.velocityMax, quaternion);

			//エミッターを生成
			ParticleEmitter* newEmitter = EmitterBuilder().SetEmitterName(emitterSettings.first).SetTranslation(translation).SetRadius(emitterSetting.radius).SetCount(emitterSetting.count)
				.SetRotate(emitterSetting.rotateMin, emitterSetting.rotateMax).SetScale(emitterSetting.scaleMin, emitterSetting.scaleMax).SetVelocity(minVelocity, maxVelocity)
				.SetLifeTime(emitterSetting.lifeTimeMin, emitterSetting.lifeTimeMax).SetColor(emitterSetting.colorMin, emitterSetting.colorMax).SetFrequency(emitterSetting.frequency)
				.SetEmitterLifeTime(emitterSetting.emitterLifeTime).SetAlignToDirection(emitterSetting.alignToDirection).SetEnableColorOverLifeTime(emitterSetting.enableColorOverLifeTime)
				.SetTargetColor(emitterSetting.targetColor).SetEnableAlphaOverLifeTime(emitterSetting.enableAlphaOverLifeTime).SetTargetAlpha(emitterSetting.targetAlpha)
				.SetEnableSizeOverLifeTime(emitterSetting.enableSizeOverLifeTime).SetTargetScale(emitterSetting.targetScale).SetEnableRotationOverLifeTime(emitterSetting.enableRotationOverLifeTime)
				.SetRotSpeed(emitterSetting.rotSpeed).SetIsBillboard(emitterSetting.isBillboard).Build();
			particleSystems_[particleSystemName]->AddParticleEmitter(newEmitter);
		}
	}
}