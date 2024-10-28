#include "ParticleEffectEditor.h"

void ParticleEffectEditor::Initialize()
{
	//パーティクルマネージャーの初期化
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Clear();
	particleManager_->SetCamera(CameraManager::GetInstance()->GetCamera("Camera"));

	//テクスチャの読み込み
	LoadTextures();

	//パーティクルシステムの作成
	CreateParticleSystems();

	//設定ファイルを読み込み
	LoadFiles();
}

void ParticleEffectEditor::Update()
{
	//新しいパーティクルエフェクトの設定を追加
	AddNewParticleEffectSetting();

	//パーティクルエフェクトが存在しなければ処理を飛ばす
	if (particleEffectConfigs_.empty()) return;

	//現在編集しているパーティクルエフェクトの設定の名前
	static std::string currentEditEffectName;

	//編集するパーティクルエフェクトの設定を選択
	SelectFromMap<ParticleEffectConfig>("Particle Effects", currentEditEffectName, particleEffectConfigs_);

	//現在のパーティクルエフェクトの設定を取得
	auto effectIt = particleEffectConfigs_.find(currentEditEffectName);
	//パーティクルエフェクトが見つからなければ処理を飛ばす
	if (effectIt == particleEffectConfigs_.end()) return;

	//選択したパーティクルエフェクトのコントロールを表示
	DisplayEffectControls(currentEditEffectName);

	//パーティクルシステムの設定を追加
	ImGui::SeparatorText("Add Particle System Setting");
	AddParticleSystemSetting(effectIt->second);

	//パーティクルシステムが存在しなければ処理を飛ばす
	if (effectIt->second.particleSystems.empty()) return;

	//パーティクルシステムの設定を編集
	EditParticleSystemSettings(effectIt->second);
}

void ParticleEffectEditor::CreateParticles(const std::string& particleEffectName, const Vector3& position, const Quaternion& quaternion)
{
	//パーティクルエフェクトが存在しなければ早期リターン
	if (particleEffectConfigs_.find(particleEffectName) == particleEffectConfigs_.end())
	{
		return;
	}

	//パーティクルエフェクトの設定を取得
	ParticleEffectConfig particleEffectSettings = particleEffectConfigs_[particleEffectName];

	//パーティクルエフェクトの設定をもとにパーティクルを生成
	for (const auto& particleSystemSettings : particleEffectSettings.particleSystems)
	{
		//パーティクルシステムの設定を取得
		const ParticleSystemSettings& particleSystemSetting = particleSystemSettings.second;

		//全てのエミッターを生成
		for (const auto& emitterSettings : particleSystemSetting.emitters)
		{
			//エミッターの設定を取得
			const EmitterSettings& emitterSetting = emitterSettings.second;

			//座標を設定
			Vector3 translation = position + Mathf::RotateVector(emitterSetting.translate, quaternion);

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
			particleSystems_[particleSystemSettings.first]->AddParticleEmitter(newEmitter);
		}

		//全ての加速フィールドを生成
		for (const auto& accelerationFieldSettings : particleSystemSetting.accelerationFields)
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
		for (const auto& gravityFieldSettings : particleSystemSetting.gravityFields)
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

void ParticleEffectEditor::LoadTextures()
{
	//テクスチャの名前を設定
	const std::vector<std::string> textureNames = {
		"smoke_01.png", "smoke_10.png", "star_07.png", "ShockWave.png", "Flare.png", "twirl_03.png", "muzzle_01.png", "Crack.png"
	};

	//テクスチャの読み込み
	for (const std::string& textureName : textureNames)
	{
		TextureManager::Load(textureName);
	}
}

void ParticleEffectEditor::CreateParticleSystems()
{
	//デフォルトのパーティクルの作成
	CreateParticleSystemForPlane("Normal", "DefaultParticle.png");
	//煙のパーティクルを作成
	CreateParticleSystemForPlane("Smoke", "smoke_01.png");
	//煙の輪パーティクルを作成
	CreateParticleSystemForPlane("SmokeRing", "smoke_10.png");
	//光のパーティクルを作成
	CreateParticleSystemForPlane("Light", "star_07.png");
	//円のパーティクルを作成
	CreateParticleSystemForPlane("Circle", "ShockWave.png");
	//光線のパーティクルを作成
	CreateParticleSystemForPlane("Flare", "Flare.png");
	//渦巻きのパーティクルを作成
	CreateParticleSystemForPlane("Twirl", "twirl_03.png");
	//フラッシュのパーティクルを作成
	CreateParticleSystemForPlane("Flash", "muzzle_01.png");
	//割れ目のパーティクルを作成
	CreateParticleSystemForPlane("Crack", "Crack.png", true);
	//岩のパーティクルを作成
	CreateParticleSystemForMesh("Rock", "Rock");
	//火花のパーティクルを作成
	CreateParticleSystemForMesh("Spark", "Cube", false, false);
}

void ParticleEffectEditor::CreateParticleSystemForPlane(const std::string& systemName, const std::string& textureName, const bool enableDepthWrite)
{
	//パーティクルシステムの作成
	particleSystems_[systemName] = ParticleManager::Create(systemName);
	particleSystems_[systemName]->SetTexture(textureName);
	particleSystems_[systemName]->SetEnableDepthWrite(enableDepthWrite);
}

void ParticleEffectEditor::CreateParticleSystemForMesh(const std::string& systemName, const std::string& modelName, const bool enableDepthWrite, const bool enableLighting)
{
	//パーティクルシステムの作成
	ParticleSystem* particleSystem = ParticleManager::Create(systemName);
	particleSystem->SetModel(modelName);
	particleSystem->SetEnableDepthWrite(enableDepthWrite);
	//ライティングの設定
	Model* model = particleSystem->GetModel();
	for (int32_t i = 0; i < model->GetNumMaterials(); ++i)
	{
		model->GetMaterial(i)->SetEnableLighting(enableLighting);
	}
	//コンテナに追加
	particleSystems_[systemName] = particleSystem;
}

void ParticleEffectEditor::AddNewParticleEffectSetting()
{
	//名前入力用のバッファ
	static char newParticleEffectSettingName[128] = { '\0' };

	//新しいパーティクルエフェクトの設定名を入力
	ImGui::InputText("New Particle Effect Name", newParticleEffectSettingName, sizeof(newParticleEffectSettingName));

	//新しいパーティクルエフェクトの設定を追加
	if (ImGui::Button("Add Particle Effect"))
	{
		//名前をstd::stringに格納
		std::string particleEffectName(newParticleEffectSettingName);

		//名前が入力されていないまたは同じ名前のパーティクルエフェクトの設定が存在していた場合は飛ばす
		if (particleEffectName.empty() || particleEffectConfigs_.find(particleEffectName) != particleEffectConfigs_.end())
		{
			return;
		}

		//新しいパーティクルエフェクトの設定を追加
		particleEffectConfigs_[particleEffectName] = ParticleEffectConfig();
		//入力バッファのクリア
		std::memset(newParticleEffectSettingName, '\0', sizeof(newParticleEffectSettingName));
	}
}

void ParticleEffectEditor::DisplayEffectControls(const std::string& effectName)
{
	//保存
	if (ImGui::Button("Save"))
	{
		SaveFile(effectName);
	}

	//次のアイテムを同じ行に配置
	ImGui::SameLine();

	//読み込み
	if (ImGui::Button("Load"))
	{
		LoadFile(effectName);
	}

	//現在編集中のパーティクルを生成
	if (ImGui::Button("CreateParticles"))
	{
		CreateParticles(effectName, { 0.0f,0.0f,0.0f }, Mathf::IdentityQuaternion());
	}
}

void ParticleEffectEditor::SaveFile(const std::string& particleEffectName)
{
	//rootのjsonオブジェクトを作成
	nlohmann::json root = nlohmann::json::object();

	//全てのパーティクルシステムのjsonオブジェクトを作成
	nlohmann::json systemsJson = nlohmann::json::object();

	//全てのパーティクルシステムの設定を保存
	for (const auto& particleSystemSettings : particleEffectConfigs_[particleEffectName].particleSystems)
	{
		//パーティクルシステムのjsonオブジェクトを作成
		nlohmann::json systemJson = nlohmann::json::object();

		//パーティクルシステムの設定を取得
		const ParticleSystemSettings& particleSystemSetting = particleSystemSettings.second;

		//全てのエミッターのjsonオブジェクトを作成
		nlohmann::json emittersJson = nlohmann::json::object();

		//全てのエミッターの設定を保存
		for (const auto& emitterSettings : particleSystemSetting.emitters)
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
			emittersJson[emitterSettings.first] = emitterJson;
		}

		//パーティクルシステムのjsonに全てのエミッターのjsonオブジェクトを追加
		systemJson["Emitters"] = emittersJson;

		//全ての加速フィールドのjsonオブジェクトを作成
		nlohmann::json accelerationFieldsJson = nlohmann::json::object();

		//全ての加速フィールドの設定を保存
		for (const auto& accelerationFieldSettings : particleSystemSetting.accelerationFields)
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
		for (const auto& gravityFieldSettings : particleSystemSetting.gravityFields)
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

void ParticleEffectEditor::LoadFiles()
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

void ParticleEffectEditor::LoadFile(const std::string& particleEffectName)
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
	ParticleEffectConfig& particleEffectSettings = particleEffectConfigs_[particleEffectName];
	particleEffectSettings.particleSystems.clear();

	//各パーティクルシステムについて
	for (nlohmann::json::iterator systemItem = particleEffectJson["ParticleSystems"].begin(); systemItem != particleEffectJson["ParticleSystems"].end(); ++systemItem)
	{
		//パーティクルシステムの名前を取得
		const std::string& systemName = systemItem.key();
		//パーティクルシステムのデータを取得
		nlohmann::json systemData = systemItem.value();

		//パーティクルシステムの設定を取得
		ParticleSystemSettings& particleSystemSettings = particleEffectSettings.particleSystems[systemName];

		//エミッターの設定を読み込む
		SetEmitterSettings(particleSystemSettings, systemData);

		//加速フィールドの設定を読み込む
		SetAccelerationFieldSettings(particleSystemSettings, systemData);

		//重力フィールドの設定を読み込む
		SetGravityFieldSettings(particleSystemSettings, systemData);
	}
}

void ParticleEffectEditor::SetEmitterSettings(ParticleSystemSettings& particleSystemSettings, nlohmann::json& systemData)
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
		EmitterSettings& emitterSettings = particleSystemSettings.emitters[emitterName];
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
	}
}

void ParticleEffectEditor::SetAccelerationFieldSettings(ParticleSystemSettings& particleSystemSettings, nlohmann::json& systemData)
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
		AccelerationFieldSettings& accelerationFieldSettings = particleSystemSettings.accelerationFields[accelerationFieldName];
		accelerationFieldSettings.deathTimer = accelerationFieldData["LifeTime"].get<float>();
		accelerationFieldSettings.translate = { accelerationFieldData["Translation"][0].get<float>(), accelerationFieldData["Translation"][1].get<float>(), accelerationFieldData["Translation"][2].get<float>() };
		accelerationFieldSettings.min = { accelerationFieldData["Min"][0].get<float>(), accelerationFieldData["Min"][1].get<float>(), accelerationFieldData["Min"][2].get<float>() };
		accelerationFieldSettings.max = { accelerationFieldData["Max"][0].get<float>(), accelerationFieldData["Max"][1].get<float>(), accelerationFieldData["Max"][2].get<float>() };
		accelerationFieldSettings.acceleration = { accelerationFieldData["Acceleration"][0].get<float>(), accelerationFieldData["Acceleration"][1].get<float>(), accelerationFieldData["Acceleration"][2].get<float>() };
	}
}

void ParticleEffectEditor::SetGravityFieldSettings(ParticleSystemSettings& particleSystemSettings, nlohmann::json& systemData)
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
		GravityFieldSettings& gravityFieldSettings = particleSystemSettings.gravityFields[gravityFieldName];
		gravityFieldSettings.deathTimer = gravityFieldData["LifeTime"].get<float>();
		gravityFieldSettings.translate = { gravityFieldData["Translation"][0].get<float>(), gravityFieldData["Translation"][1].get<float>(), gravityFieldData["Translation"][2].get<float>() };
		gravityFieldSettings.min = { gravityFieldData["Min"][0].get<float>(), gravityFieldData["Min"][1].get<float>(), gravityFieldData["Min"][2].get<float>() };
		gravityFieldSettings.max = { gravityFieldData["Max"][0].get<float>(), gravityFieldData["Max"][1].get<float>(), gravityFieldData["Max"][2].get<float>() };
		gravityFieldSettings.strength = gravityFieldData["Strength"].get<float>();
		gravityFieldSettings.stopDistance = gravityFieldData["StopDistance"].get<float>();
	}
}

void ParticleEffectEditor::AddParticleSystemSetting(ParticleEffectConfig& selectedEffect)
{
	//選択パーティクルシステムの設定の名前
	static std::string selectedSystemName;

	//編集するパーティクルシステムの設定を選択
	SelectFromMap<ParticleSystem*>("Particle Systems", selectedSystemName, particleSystems_);

	//同じパーティクルシステムの設定が存在しなければ新しい設定を追加
	if (ImGui::Button("Add Particle System"))
	{
		if (selectedEffect.particleSystems.find(selectedSystemName) == selectedEffect.particleSystems.end())
		{
			selectedEffect.particleSystems[selectedSystemName] = ParticleSystemSettings();
		}
	}
}

void ParticleEffectEditor::EditParticleSystemSettings(ParticleEffectConfig& selectedEffect)
{
	//現在編集しているパーティクルシステムの設定の名前
	static std::string currentEditSystemName;

	//パーティクルシステムの設定
	EditSettingsSection<ParticleSystemSettings>("Edit Particle Systems", selectedEffect.particleSystems, currentEditSystemName, 
		[this](ParticleSystemSettings& systemSetting)
		{
			//新しいエミッターの設定を追加
			AddEmitterSetting(systemSetting);

			//エミッターの設定の編集
			if (!systemSetting.emitters.empty())
			{
				EditEmitterSettings(systemSetting);
			}

			//新しい加速フィールドの設定を追加
			AddAccelerationFieldSetting(systemSetting);

			//加速フィールドの設定を編集
			if (!systemSetting.accelerationFields.empty())
			{
				EditAccelerationFieldSettings(systemSetting);
			}

			//新しい重力フィールドの設定を追加
			AddGravityFieldSetting(systemSetting);

			//重力フィールドの設定を編集
			if (!systemSetting.accelerationFields.empty())
			{
				EditGravityFieldSettings(systemSetting);
			}
		}
	);
}

void ParticleEffectEditor::AddEmitterSetting(ParticleSystemSettings& selectedSystem)
{
	//名前入力用のバッファ
	static char newEmitterSettingName[128] = { '\0' };

	//区切り線と編集名を表示
	ImGui::NewLine();
	ImGui::SeparatorText("Emitter Settings");

	//新しいエミッターの設定名を入力
	ImGui::InputText("New Emitter Name", newEmitterSettingName, sizeof(newEmitterSettingName));

	//新しいエミッターの設定を追加
	if (ImGui::Button("Add Emitter"))
	{
		//名前をstd::stringに格納
		std::string emitterName(newEmitterSettingName);

		//名前が入力されていないまたは同じ名前のエミッターの設定が存在していた場合は飛ばす
		if (emitterName.empty() || selectedSystem.emitters.find(emitterName) != selectedSystem.emitters.end())
		{
			return;
		}

		//新しいエミッターの設定を追加
		selectedSystem.emitters[newEmitterSettingName] = EmitterSettings();
		//入力バッファのクリア
		std::memset(newEmitterSettingName, '\0', sizeof(newEmitterSettingName));
	}
}

void ParticleEffectEditor::EditEmitterSettings(ParticleSystemSettings& selectedSystem)
{
	//現在編集しているエミッターの設定の名前
	static std::string currentEditEmitterName;

	//エミッターの設定
	EditSettingsSection<EmitterSettings>("Particle Emitters", selectedSystem.emitters, currentEditEmitterName,
		[](EmitterSettings& emitterSetting)
		{
			ImGui::DragFloat("Emitter LifeTime", &emitterSetting.emitterLifeTime, 0.01f);
			ImGui::DragFloat3("Translation", &emitterSetting.translate.x, 0.01f);
			ImGui::DragFloat("Emission Radius", &emitterSetting.radius, 0.01f);
			ImGui::DragInt("Particle Count", &emitterSetting.count);
			ImGui::DragFloat("Frequency", &emitterSetting.frequency, 0.01f);
			ImGui::Checkbox("Align to Direction", &emitterSetting.alignToDirection);
			ImGui::Checkbox("Billboard Mode", &emitterSetting.isBillboard);
			ImGui::DragFloat3("Min Rotation", &emitterSetting.rotateMin.x, 0.01f);
			ImGui::DragFloat3("Max Rotation", &emitterSetting.rotateMax.x, 0.01f);
			ImGui::DragFloat3("Min Scale", &emitterSetting.scaleMin.x, 0.01f);
			ImGui::DragFloat3("Max Scale", &emitterSetting.scaleMax.x, 0.01f);
			ImGui::DragFloat3("Min Velocity", &emitterSetting.velocityMin.x, 0.01f);
			ImGui::DragFloat3("Max Velocity", &emitterSetting.velocityMax.x, 0.01f);
			ImGui::DragFloat("Min Particle LifeTime", &emitterSetting.lifeTimeMin, 0.01f);
			ImGui::DragFloat("Max Particle LifeTime", &emitterSetting.lifeTimeMax, 0.01f);
			ImGui::ColorEdit4("Min Color", &emitterSetting.colorMin.x);
			ImGui::ColorEdit4("Max Color", &emitterSetting.colorMax.x);
			ImGui::Checkbox("Enable Color Over Lifetime", &emitterSetting.enableColorOverLifeTime);
			if (emitterSetting.enableColorOverLifeTime)
			{
				ImGui::ColorEdit3("Target Color", &emitterSetting.targetColor.x);
			}
			ImGui::Checkbox("Enable Alpha Over Lifetime", &emitterSetting.enableAlphaOverLifeTime);
			if (emitterSetting.enableAlphaOverLifeTime)
			{
				ImGui::DragFloat("Target Alpha", &emitterSetting.targetAlpha, 0.01f, 0.0f, 1.0f);
			}
			ImGui::Checkbox("Enable Size Over Lifetime", &emitterSetting.enableSizeOverLifeTime);
			if (emitterSetting.enableSizeOverLifeTime)
			{
				ImGui::DragFloat3("Target Scale", &emitterSetting.targetScale.x, 0.01f, 0.0f);
			}
			ImGui::Checkbox("Enable Rotation Over Lifetime", &emitterSetting.enableRotationOverLifeTime);
			if (emitterSetting.enableRotationOverLifeTime)
			{
				ImGui::DragFloat3("Rotation Speed", &emitterSetting.rotSpeed.x, 0.01f, -360.0f, 360.0f);
			}
		}
	);
}

void ParticleEffectEditor::AddAccelerationFieldSetting(ParticleSystemSettings& selectedSystem)
{
	//名前入力用のバッファ
	static char newAccelerationFieldSettingName[128] = { '\0' };

	//区切り線と編集名を表示
	ImGui::NewLine();
	ImGui::SeparatorText("Acceleration Field Settings");

	//新しい加速フィールドの設定名を入力
	ImGui::InputText("New Acceleration Field Name", newAccelerationFieldSettingName, sizeof(newAccelerationFieldSettingName));

	//新しい加速フィールドの設定を追加
	if (ImGui::Button("Add Acceleration Field"))
	{
		//名前をstd::stringに格納
		std::string accelerationFieldName(newAccelerationFieldSettingName);

		//名前が入力されていないまたは同じ名前の加速フィールドの設定が存在していた場合は飛ばす
		if (accelerationFieldName.empty() || selectedSystem.accelerationFields.find(accelerationFieldName) != selectedSystem.accelerationFields.end())
		{
			return;
		}

		//新しい加速フィールドの設定を追加
		selectedSystem.accelerationFields[accelerationFieldName] = AccelerationFieldSettings();
		//入力バッファのクリア
		std::memset(newAccelerationFieldSettingName, '\0', sizeof(newAccelerationFieldSettingName));
	}
}

void ParticleEffectEditor::EditAccelerationFieldSettings(ParticleSystemSettings& selectedSystem)
{
	//現在編集している加速フィールドの設定の名前
	static std::string currentEditAccelerationFieldName;

	//加速フィールドの設定
	EditSettingsSection<AccelerationFieldSettings>("Acceleration Fields", selectedSystem.accelerationFields, currentEditAccelerationFieldName,
		[](AccelerationFieldSettings& accelerationFieldSetting)
		{
			ImGui::DragFloat("LifeTime", &accelerationFieldSetting.deathTimer, 0.01f);
			ImGui::DragFloat3("Translation", &accelerationFieldSetting.translate.x, 0.01f);
			ImGui::DragFloat3("Min", &accelerationFieldSetting.min.x, 0.01f);
			ImGui::DragFloat3("Max", &accelerationFieldSetting.max.x, 0.01f);
			ImGui::DragFloat3("Acceleration", &accelerationFieldSetting.acceleration.x, 0.01f);
		}
	);
}

void ParticleEffectEditor::AddGravityFieldSetting(ParticleSystemSettings& selectedSystem)
{
	//名前入力用のバッファ
	static char newGravityFieldSettingName[128] = { '\0' };

	//区切り線と編集名を表示
	ImGui::NewLine();
	ImGui::SeparatorText("Gravity Field Settings");

	//新しい重力フィールドの設定名を入力
	ImGui::InputText("New Gravity Field Name", newGravityFieldSettingName, sizeof(newGravityFieldSettingName));

	//新しい重力フィールドの設定を追加
	if (ImGui::Button("Add Gravity Field"))
	{
		//名前をstd::stringに格納
		std::string gravityFieldName(newGravityFieldSettingName);

		//名前が入力されていないまたは同じ名前の重力フィールドの設定が存在していた場合は飛ばす
		if (gravityFieldName.empty() || selectedSystem.gravityFields.find(gravityFieldName) != selectedSystem.gravityFields.end())
		{
			return;
		}

		//新しい重力フィールドの設定を追加
		selectedSystem.gravityFields[gravityFieldName] = GravityFieldSettings();
		//入力バッファのクリア
		std::memset(newGravityFieldSettingName, '\0', sizeof(newGravityFieldSettingName));
	}
}

void ParticleEffectEditor::EditGravityFieldSettings(ParticleSystemSettings& selectedSystem)
{
	//現在編集している重力フィールドの設定の名前
	static std::string currentEditGravityFieldName;

	//重力フィールドの設定
	EditSettingsSection<GravityFieldSettings>("Gravity Fields", selectedSystem.gravityFields, currentEditGravityFieldName,
		[](GravityFieldSettings& gravityFieldSetting)
		{
			ImGui::DragFloat("LifeTime", &gravityFieldSetting.deathTimer, 0.01f);
			ImGui::DragFloat3("Translation", &gravityFieldSetting.translate.x, 0.01f);
			ImGui::DragFloat3("Min", &gravityFieldSetting.min.x, 0.01f);
			ImGui::DragFloat3("Max", &gravityFieldSetting.max.x, 0.01f);
			ImGui::DragFloat("Strength", &gravityFieldSetting.strength, 0.01f);
			ImGui::DragFloat("StopDistance", &gravityFieldSetting.stopDistance, 0.01f);
		}
	);
}