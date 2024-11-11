#include "CombatAnimationEditor.h"
#include "Application/Src/Object/Character/BaseCharacter.h"

const int32_t AnimationController::GetAnimationEventCount(const EventType eventType) const
{
	int32_t animationEventCount = 0;
	for (const std::shared_ptr<AnimationEvent>& animationEvent : animationEvents)
	{
		if (animationEvent->eventType == eventType)
		{
			animationEventCount++;
		}
	}
	return animationEventCount;
}

void CombatAnimationEditor::Initialize()
{
	//全てのファイルの読み込み
	LoadFiles();
}

void CombatAnimationEditor::Update()
{
	//キャラクタータブを更新
	if (ImGui::BeginTabBar("Characters"))
	{
		//各キャラクターのアニメーション編集
		for (const auto& editableCharacter : editableCharacters_)
		{
			//アイテムバーを表示
			if (ImGui::BeginTabItem(editableCharacter.first.c_str()))
			{
				//キャラクターのアニメーションを編集
				EditCharacterAnimations(editableCharacter.second);

				//アイテムバーを終了
				ImGui::EndTabItem();
			}
		}
		//タブバーを終了
		ImGui::EndTabBar();
	}
}

void CombatAnimationEditor::AddEditableCharacter(BaseCharacter* character)
{
	//キャラクター名を取得
	const std::string& characterName = character->GetName();

	//同じキャラクターが存在していない場合は追加
	if (editableCharacters_.find(characterName) == editableCharacters_.end())
	{
		editableCharacters_[characterName] = character;
	}

	//同じアニメーションデータが存在しない場合は追加
	if (characterAnimations_.find(characterName) == characterAnimations_.end())
	{
		characterAnimations_[characterName] = CharacterAnimationData();
	}
}

const AnimationController& CombatAnimationEditor::GetAnimationController(const std::string& characterName, const std::string& animationName) const
{
	//キャラクターアニメーションを探す
	auto characterAnimation = characterAnimations_.find(characterName);
	//キャラクターアニメーションが見つかった場合
	if (characterAnimation != characterAnimations_.end())
	{
		//アニメーションコントローラーを探す
		auto animationController = characterAnimation->second.animationControllers.find(animationName);
		//アニメーションコントローラーが見つかった場合はそれを返す
		if (animationController != characterAnimation->second.animationControllers.end())
		{
			return animationController->second;
		}
	}
	//見つからなかった場合はデフォルトの値を返す
	static AnimationController defaultAnimationController{};
	return defaultAnimationController;
}

void CombatAnimationEditor::EditCharacterAnimations(BaseCharacter* character)
{
	//キャラクターのアニメーションのデータを取得
	CharacterAnimationData& characterAnimationData = characterAnimations_[character->GetName()];

	//キャラクターのアニメーターを取得
	AnimatorComponent* animator = character->GetComponent<AnimatorComponent>();

	//アニメーションを選択
	SelectFromMap("Animations", characterAnimationData.selectedAnimation, animator->GetAnimations(), true);

	//選択しているアニメーションがない場合は処理を飛ばす
	if (characterAnimationData.selectedAnimation.empty()) { return; };

	//デバッグモードの設定
	bool isDebug = character->GetIsDebug();
	ToggleDebugMode(character, characterAnimationData, isDebug);

	//アニメーションの時間を編集
	EditAnimationTime(animator, characterAnimationData.selectedAnimation, isDebug);

	//アニメーションコントローラーを取得
	AnimationController& animationController = characterAnimationData.animationControllers[characterAnimationData.selectedAnimation];

	//固定軸を設定
	ImGui::DragFloat3("In Place Axis", &animationController.inPlaceAxis.x, 1.0f, 0.0f, 1.0f);

	//選択した戦闘アニメーションのコントロールを表示
	DisplayAnimationControllerControls(character->GetName(), characterAnimationData.selectedAnimation);

	//アニメーション速度の設定を管理
	ManageAnimationSpeedConfigs(animationController.animationSpeedConfigs);

	//アニメーションイベントを管理
	ManageAnimationEvents(animationController.animationEvents);
}

void CombatAnimationEditor::ToggleDebugMode(BaseCharacter* character, CharacterAnimationData& characterAnimationData, bool isDebug)
{
	//デバッグフラグの切り替え
	if (ImGui::Checkbox("IsDebug", &isDebug))
	{
		isDebug ? character->StartDebugMode(characterAnimationData.selectedAnimation) : character->EndDebugMode();
	}
}

void CombatAnimationEditor::EditAnimationTime(AnimatorComponent* animator, const std::string& selectedAnimationName, const bool isDebug)
{
	//現在編集しているアニメーションの時間を編集
	static float currentEditAnimationTime;
	ImGui::SliderFloat("Animation Time", &currentEditAnimationTime, 0.0f, animator->GetAnimationDuration(selectedAnimationName));

	//デバッグ中の場合はアニメーターにアニメーションの時間を設定
	if (isDebug)
	{
		animator->SetAnimationTime(selectedAnimationName, currentEditAnimationTime);
	}
}

void CombatAnimationEditor::DisplayAnimationControllerControls(const std::string& characterName, const std::string& animationControllerName)
{
	//保存
	if (ImGui::Button("Save"))
	{
		SaveFile(characterName, animationControllerName);
	}

	//次のアイテムを同じ行に配置
	ImGui::SameLine();

	//読み込み
	if (ImGui::Button("Load"))
	{
		LoadFile(characterName, animationControllerName);
	}
}

void CombatAnimationEditor::SaveFile(const std::string& characterName, const std::string& animationControllerName)
{
	//アニメーションコントローラーを取得
	const AnimationController& animationController = characterAnimations_[characterName].animationControllers[animationControllerName];

	//jsonの構造体作成
	nlohmann::json root = nlohmann::json::object();
	nlohmann::json animationControllerJson = nlohmann::json::object();

	//アニメーション速度の設定を保存
	SaveAnimationSpeedConfigs(animationController.animationSpeedConfigs, animationControllerJson["AnimationSpeedConfigs"]);

	//アニメーションイベントを保存
	SaveAnimationEvents(animationController.animationEvents, animationControllerJson["AnimationEvents"]);

	//動かす軸を保存
	animationControllerJson["InPlaceAxis"] = { animationController.inPlaceAxis.x, animationController.inPlaceAxis.y, animationController.inPlaceAxis.z };

	//rootに追加
	root[characterName][animationControllerName] = animationControllerJson;

	//ディレクトリ作成
	std::string directoryPath = kDirectoryPath + characterName + "/";
	std::filesystem::path dir(directoryPath);
	if (!std::filesystem::exists(directoryPath)) std::filesystem::create_directory(directoryPath);

	//ファイルにjson文字列を書き込む
	std::ofstream ofs(directoryPath + animationControllerName + ".json");
	ofs << std::setw(4) << root << std::endl;
	//ファイルを閉じる
	ofs.close();
}

void CombatAnimationEditor::SaveAnimationSpeedConfigs(const std::vector<AnimationSpeedConfig>& animationSpeedConfigs, nlohmann::json& animationSpeedConfigsJson)
{
	//全てのアニメーション速度設定を保存
	for (int32_t i = 0; i < animationSpeedConfigs.size(); ++i)
	{
		//アニメーションイベントのjsonオブジェクトを作成
		nlohmann::json speedConfigJson = { {"Duration", animationSpeedConfigs[i].duration}, {"AnimationSpeed", animationSpeedConfigs[i].animationSpeed} };

		//アニメーション速度設定をjsonオブジェクトに書き込む
		animationSpeedConfigsJson["AnimationSpeedConfig" + std::to_string(i)] = speedConfigJson;
	}
}

void CombatAnimationEditor::SaveAnimationEvents(const std::vector<std::shared_ptr<AnimationEvent>>& animationEvents, nlohmann::json& animationEventsJson)
{
	//全てのアニメーションイベントの設定を保存
	for (int32_t i = 0; i < animationEvents.size(); ++i)
	{
		//アニメーションイベントのjsonオブジェクトを作成
		nlohmann::json eventJson = nlohmann::json::object();

		//アニメーションイベントごとに保存する項目を設定
		switch (animationEvents[i]->eventType)
		{
		case EventType::kMovement:
			SaveMovementEvent(dynamic_cast<MovementEvent*>(animationEvents[i].get()), eventJson);
			break;
		case EventType::kRotation:
			SaveRotationEvent(dynamic_cast<RotationEvent*>(animationEvents[i].get()), eventJson);
			break;
		case EventType::kAttack:
			SaveAttackEvent(dynamic_cast<AttackEvent*>(animationEvents[i].get()), eventJson);
			break;
		case EventType::kCameraAnimation:
			SaveCameraAnimationEvent(dynamic_cast<CameraAnimationEvent*>(animationEvents[i].get()), eventJson);
			break;
		case EventType::kCancel:
			SaveCancelEvent(dynamic_cast<CancelEvent*>(animationEvents[i].get()), eventJson);
			break;
		case EventType::kBufferedAction:
			SaveBufferedActionEvent(dynamic_cast<BufferedActionEvent*>(animationEvents[i].get()), eventJson);
			break;
		}

		//コンバットアニメーションのjsonオブジェクトにアニメーションイベントを追加
		animationEventsJson["AnimationEvent" + std::to_string(i)] = eventJson;
	}
}

void CombatAnimationEditor::SaveMovementEvent(const MovementEvent* movementEvent, nlohmann::json& eventJson)
{
	//イベントのタイプを設定
	eventJson.update({ {"EventType", "Movement"},{"MovementType",movementEvent->movementType == MovementType::kVelocity ? "Velocity" : "Easing"},
		{"UseStickInput", movementEvent->useStickInput}, {"IsProximityStopEnabled", movementEvent->isProximityStopEnabled},{"MoveTowardsEnemy", movementEvent->moveTowardsEnemy},
		{"RotateTowardsMovement", movementEvent->rotateTowardsMovement}, {"StartEventTime", movementEvent->startEventTime},{"EndEventTime", movementEvent->endEventTime} });

	//移動イベントを保存
	movementEvent->movementType == MovementType::kVelocity ? SaveVelocityMovementEvent(static_cast<const VelocityMovementEvent*>(movementEvent), eventJson) :
		SaveEasingMovementEvent(static_cast<const EasingMovementEvent*>(movementEvent), eventJson);
}

void CombatAnimationEditor::SaveVelocityMovementEvent(const VelocityMovementEvent* velocityMovementEvent, nlohmann::json& eventJson)
{
	//パラメーターを保存
	eventJson["Velocity"] = { velocityMovementEvent->velocity.x, velocityMovementEvent->velocity.y, velocityMovementEvent->velocity.z };
}

void CombatAnimationEditor::SaveEasingMovementEvent(const EasingMovementEvent* easingMovementEvent, nlohmann::json& eventJson)
{
	//パラメーターを保存
	eventJson["TargetPosition"] = { easingMovementEvent->targetPosition.x, easingMovementEvent->targetPosition.y, easingMovementEvent->targetPosition.z };
	eventJson["EasingType"] = EASING_TYPES[static_cast<int>(easingMovementEvent->easingType)];
}

void CombatAnimationEditor::SaveRotationEvent(const RotationEvent* rotationEvent, nlohmann::json& eventJson)
{
	//パラメーターを保存
	eventJson.update({ {"EventType", "Rotation"},{"StartEventTime", rotationEvent->startEventTime},{"EndEventTime", rotationEvent->endEventTime},
		{"RotationAxis", {rotationEvent->rotationAxis.x, rotationEvent->rotationAxis.y, rotationEvent->rotationAxis.z}},{"RotationAngle", rotationEvent->rotationAngle},
		{"EasingType", EASING_TYPES[static_cast<int>(rotationEvent->easingType)]} });
}

void CombatAnimationEditor::SaveAttackEvent(const AttackEvent* attackEvent, nlohmann::json& eventJson)
{
	//各パラメーターを取得
	const AttackParameters& attackParams = attackEvent->attackParameters;
	const HitboxParameters& hitboxParams = attackEvent->hitboxParameters;
	const KnockbackParameters& knockbackParams = attackEvent->knockbackParameters;
	const HitEffectConfig& hitEffectConfig = attackEvent->effectConfigs;

	//パラメーターを保存
	eventJson.update({ {"EventType", "Attack"},{"StartEventTime", attackEvent->startEventTime},{"EndEventTime", attackEvent->endEventTime},
		{"MaxHitCount", attackParams.maxHitCount},{"HitInterval", attackParams.hitInterval},{"Damage", attackParams.damage},
		{"HitboxCenter", {hitboxParams.center.x, hitboxParams.center.y, hitboxParams.center.z}},{"HitboxSize", {hitboxParams.size.x, hitboxParams.size.y, hitboxParams.size.z}},
		{"KnockbackVelocity", {knockbackParams.velocity.x, knockbackParams.velocity.y, knockbackParams.velocity.z}},{"KnockbackAcceleration", {knockbackParams.acceleration.x, knockbackParams.acceleration.y, knockbackParams.acceleration.z}},
		{"HitStopDuration", hitEffectConfig.hitStopDuration},{"CameraShakeDuration", hitEffectConfig.cameraShakeDuration},{"CameraShakeIntensity", {hitEffectConfig.cameraShakeIntensity.x, hitEffectConfig.cameraShakeIntensity.y, hitEffectConfig.cameraShakeIntensity.z}},
		{"HitParticleName", hitEffectConfig.hitParticleName},{"HitSEType", HIT_SE_TYPES[static_cast<int>(hitEffectConfig.hitSEType)]},{"ReactionType", REACTION_TYPES[static_cast<int>(hitEffectConfig.reactionType)]} });
}

void CombatAnimationEditor::SaveCameraAnimationEvent(const CameraAnimationEvent* cameraAnimationEvent, nlohmann::json& eventJson)
{
	eventJson.update({ {"EventType", "CameraAnimation"},{"StartEventTime", cameraAnimationEvent->startEventTime},{"EndEventTime", cameraAnimationEvent->endEventTime},
		{"AnimationName", cameraAnimationEvent->animationName},{"AnimationSpeed", cameraAnimationEvent->animationSpeed},{"SyncWithCharacterAnimation", cameraAnimationEvent->syncWithCharacterAnimation},
		{"CameraAnimationTrigger", CAMERA_ANIMATION_TRIGGERS[static_cast<int>(cameraAnimationEvent->trigger)]} });
}

void CombatAnimationEditor::SaveCancelEvent(const CancelEvent* cancelEvent, nlohmann::json& eventJson)
{
	//パラメーターを保存
	eventJson.update({ {"EventType", "Cancel"},{"StartEventTime", cancelEvent->startEventTime},{"EndEventTime", cancelEvent->endEventTime},{"CancelType", cancelEvent->cancelType} });
}

void CombatAnimationEditor::SaveBufferedActionEvent(const BufferedActionEvent* bufferedActionEvent, nlohmann::json& eventJson)
{
	//パラメーターを保存
	eventJson.update({ {"EventType", "BufferedAction"},{"StartEventTime", bufferedActionEvent->startEventTime},{"EndEventTime", bufferedActionEvent->endEventTime},{"BufferedActionType", bufferedActionEvent->bufferedActionType} });
}


void CombatAnimationEditor::LoadFiles()
{
	//保存先ディレクトリパス
	std::filesystem::path dir(kDirectoryPath);
	//ディレクトリが無ければスキップする
	if (!std::filesystem::exists(kDirectoryPath)) return;

	//ディレクトリを検索
	std::filesystem::recursive_directory_iterator dir_it(kDirectoryPath);
	for (const std::filesystem::directory_entry& entry : dir_it)
	{
		//ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();
		//.jsonファイル以外はスキップ
		if (filePath.extension() != ".json") continue;
		//ファイルを読み込む
		LoadFile(filePath.parent_path().filename().string(), filePath.stem().string());
	}
}

void CombatAnimationEditor::LoadFile(const std::string& characterName, const std::string& animationControllerName)
{
	//読み込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + characterName + "/" + animationControllerName + ".json";
	//読み込み用のファイルストリーム
	std::ifstream ifs(filePath);
	//ファイルオープン失敗
	if (ifs.fail())
	{
		MessageBoxA(nullptr, "Failed to open data file for read.", "CombatAnimationEditor", 0);
		assert(0);
		return;
	}

	//json文字列からjsonのデータ構造に展開
	nlohmann::json root;
	ifs >> root;
	//ファイルを閉じる
	ifs.close();

	//jsonに何も書かれていなければ処理を飛ばす
	if (root.empty()) { return; };

	//アニメーションコントローラーのデータを取得
	nlohmann::json animationControllerJson = root[characterName][animationControllerName];

	//アニメーションコントローラーのデータを取得
	AnimationController& animationController = characterAnimations_[characterName].animationControllers[animationControllerName];

	//アニメーション速度設定を読み込む
	animationController.animationSpeedConfigs.clear();
	LoadAnimationSpeedConfigs(animationController.animationSpeedConfigs, animationControllerJson["AnimationSpeedConfigs"]);

	//アニメーションイベントを読み込む
	animationController.animationEvents.clear();
	LoadAnimationEvents(animationController.animationEvents, animationControllerJson["AnimationEvents"]);

	//動かす軸を読み込む
	animationController.inPlaceAxis = { animationControllerJson["InPlaceAxis"][0].get<float>(), animationControllerJson["InPlaceAxis"][1].get<float>(), animationControllerJson["InPlaceAxis"][2].get<float>() };
}

void CombatAnimationEditor::LoadAnimationSpeedConfigs(std::vector<AnimationSpeedConfig>& animationSpeedConfigs, nlohmann::json& animationSpeedConfigsJson)
{
	//全てのアニメーション速度設定を読み込む
	for (const nlohmann::json& speedConfig : animationSpeedConfigsJson)
	{
		//追加するアニメーション速度設定
		AnimationSpeedConfig animationSpeedConfig{};

		//持続時間を取得
		animationSpeedConfig.duration = speedConfig["Duration"];

		//アニメーションの速度を取得
		animationSpeedConfig.animationSpeed = speedConfig["AnimationSpeed"];

		//アニメーション速度設定を追加
		animationSpeedConfigs.push_back(animationSpeedConfig);
	}
}

void CombatAnimationEditor::LoadAnimationEvents(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents, nlohmann::json& eventJson)
{
	//全てのアニメーションイベントを読み込む
	for (const nlohmann::json& eventItem : eventJson)
	{
		//イベントタイプを取得
		std::string eventType = eventItem["EventType"];

		//新しく追加するアニメーションイベント
		std::shared_ptr<AnimationEvent> animationEvent;

		//移動イベントを読み込む
		if (eventType == "Movement")
		{
			animationEvent = LoadMovementEvent(eventItem);
		}
		//回転イベントを読み込む
		else if (eventType == "Rotation")
		{
			animationEvent = LoadRotationEvent(eventItem);
		}
		//攻撃イベントを読み込む
		else if (eventType == "Attack")
		{
			animationEvent = LoadAttackEvent(eventItem);
		}
		//カメラアニメーションイベントを読み込む
		else if (eventType == "CameraAnimation")
		{
			animationEvent = LoadCameraAnimationEvent(eventItem);
		}
		//キャンセルイベントを読み込む
		else if (eventType == "Cancel")
		{
			animationEvent = LoadCancelEvent(eventItem);
		}
		//先行入力イベントを読み込む
		else if (eventType == "BufferedAction")
		{
			animationEvent = LoadBufferedActionEvent(eventItem);
		}

		//イベントが生成されていたらアニメーションイベントを追加
		if (animationEvent)
		{
			animationEvents.push_back(animationEvent);
		}
	}
}

std::shared_ptr<MovementEvent> CombatAnimationEditor::LoadMovementEvent(const nlohmann::json& eventJson) const
{
	// 移動タイプを取得
	std::string movementType = eventJson["MovementType"];

	// 移動タイプに応じて読み込むパラメーターを変更
	if (movementType == "Velocity")
	{
		return LoadVelocityMovementEvent(eventJson);
	}
	else if (movementType == "Easing")
	{
		return LoadEasingMovementEvent(eventJson);
	}
	return nullptr;
}

void CombatAnimationEditor::InitializeCommonMovementEvent(const std::shared_ptr<MovementEvent>& movementEvent, const nlohmann::json& eventJson) const
{
	movementEvent->eventType = EventType::kMovement;
	movementEvent->useStickInput = eventJson["UseStickInput"];
	movementEvent->isProximityStopEnabled = eventJson["IsProximityStopEnabled"];
	movementEvent->moveTowardsEnemy = eventJson["MoveTowardsEnemy"];
	movementEvent->rotateTowardsMovement = eventJson["RotateTowardsMovement"];
	movementEvent->startEventTime = eventJson["StartEventTime"];
	movementEvent->endEventTime = eventJson["EndEventTime"];
}

std::shared_ptr<VelocityMovementEvent> CombatAnimationEditor::LoadVelocityMovementEvent(const nlohmann::json& eventJson) const
{
	std::shared_ptr<VelocityMovementEvent> velocityMovementEvent = std::make_shared<VelocityMovementEvent>();
	InitializeCommonMovementEvent(velocityMovementEvent, eventJson);
	velocityMovementEvent->movementType = MovementType::kVelocity;
	velocityMovementEvent->velocity = { eventJson["Velocity"][0].get<float>(), eventJson["Velocity"][1].get<float>(), eventJson["Velocity"][2].get<float>() };
	return velocityMovementEvent;
}

std::shared_ptr<EasingMovementEvent> CombatAnimationEditor::LoadEasingMovementEvent(const nlohmann::json& eventJson) const
{
	std::shared_ptr<EasingMovementEvent> easingMovementEvent = std::make_shared<EasingMovementEvent>();
	InitializeCommonMovementEvent(easingMovementEvent, eventJson);
	easingMovementEvent->movementType = MovementType::kEasing;
	easingMovementEvent->targetPosition = { eventJson["TargetPosition"][0].get<float>(), eventJson["TargetPosition"][1].get<float>(), eventJson["TargetPosition"][2].get<float>() };
	for (int32_t i = 0; i < IM_ARRAYSIZE(EASING_TYPES); ++i)
	{
		if (eventJson["EasingType"] == EASING_TYPES[i])
		{
			easingMovementEvent->easingType = static_cast<EasingType>(i);
		}
	}
	return easingMovementEvent;
}

std::shared_ptr<RotationEvent> CombatAnimationEditor::LoadRotationEvent(const nlohmann::json& eventJson) const
{
	std::shared_ptr<RotationEvent> rotationEvent = std::make_shared<RotationEvent>();
	rotationEvent->eventType = EventType::kRotation;
	rotationEvent->startEventTime = eventJson["StartEventTime"];
	rotationEvent->endEventTime = eventJson["EndEventTime"];
	rotationEvent->rotationAngle = eventJson["RotationAngle"];
	for (int32_t i = 0; i < IM_ARRAYSIZE(EASING_TYPES); ++i)
	{
		if (eventJson["EasingType"] == EASING_TYPES[i])
		{
			rotationEvent->easingType = static_cast<EasingType>(i);
		}
	}
	return rotationEvent;
}

std::shared_ptr<AttackEvent> CombatAnimationEditor::LoadAttackEvent(const nlohmann::json& eventJson) const
{
	std::shared_ptr<AttackEvent> attackEvent = std::make_shared<AttackEvent>();
	attackEvent->eventType = EventType::kAttack;
	attackEvent->startEventTime = eventJson["StartEventTime"];
	attackEvent->endEventTime = eventJson["EndEventTime"];
	attackEvent->attackParameters.maxHitCount = eventJson["MaxHitCount"];
	attackEvent->attackParameters.hitInterval = eventJson["HitInterval"];
	attackEvent->attackParameters.damage = eventJson["Damage"];
	attackEvent->hitboxParameters.center = { eventJson["HitboxCenter"][0].get<float>(), eventJson["HitboxCenter"][1].get<float>(), eventJson["HitboxCenter"][2].get<float>() };
	attackEvent->hitboxParameters.size = { eventJson["HitboxSize"][0].get<float>(), eventJson["HitboxSize"][1].get<float>(), eventJson["HitboxSize"][2].get<float>() };
	attackEvent->knockbackParameters.velocity = { eventJson["KnockbackVelocity"][0].get<float>(), eventJson["KnockbackVelocity"][1].get<float>(), eventJson["KnockbackVelocity"][2].get<float>() };
	attackEvent->knockbackParameters.acceleration = { eventJson["KnockbackAcceleration"][0].get<float>(), eventJson["KnockbackAcceleration"][1].get<float>(), eventJson["KnockbackAcceleration"][2].get<float>() };
	attackEvent->effectConfigs.hitStopDuration = eventJson["HitStopDuration"];
	attackEvent->effectConfigs.cameraShakeDuration = eventJson["CameraShakeDuration"];
	attackEvent->effectConfigs.cameraShakeIntensity = { eventJson["CameraShakeIntensity"][0].get<float>(), eventJson["CameraShakeIntensity"][1].get<float>(), eventJson["CameraShakeIntensity"][2].get<float>() };
	attackEvent->effectConfigs.hitParticleName = eventJson["HitParticleName"];
	for (int32_t i = 0; i < IM_ARRAYSIZE(HIT_SE_TYPES); ++i)
	{
		if (eventJson["HitSEType"] == HIT_SE_TYPES[i])
		{
			attackEvent->effectConfigs.hitSEType = static_cast<HitSEType>(i);
		}
	}
	for (int32_t i = 0; i < IM_ARRAYSIZE(REACTION_TYPES); ++i)
	{
		if (eventJson["ReactionType"] == REACTION_TYPES[i])
		{
			attackEvent->effectConfigs.reactionType = static_cast<ReactionType>(i);
		}
	}
	return attackEvent;
}

std::shared_ptr<CameraAnimationEvent> CombatAnimationEditor::LoadCameraAnimationEvent(const nlohmann::json& eventJson) const
{
	std::shared_ptr<CameraAnimationEvent> cameraAnimationEvent = std::make_shared<CameraAnimationEvent>();
	cameraAnimationEvent->eventType = EventType::kCameraAnimation;
	cameraAnimationEvent->startEventTime = eventJson["StartEventTime"];
	cameraAnimationEvent->endEventTime = eventJson["EndEventTime"];
	cameraAnimationEvent->animationName = eventJson["AnimationName"];
	cameraAnimationEvent->animationSpeed = eventJson["AnimationSpeed"];
	cameraAnimationEvent->syncWithCharacterAnimation = eventJson["SyncWithCharacterAnimation"];
	for (int32_t i = 0; i < IM_ARRAYSIZE(CAMERA_ANIMATION_TRIGGERS); ++i)
	{
		if (eventJson["CameraAnimationTrigger"] == CAMERA_ANIMATION_TRIGGERS[i])
		{
			cameraAnimationEvent->trigger = static_cast<CameraAnimationTrigger>(i);
		}
	}
	return cameraAnimationEvent;
}

std::shared_ptr<CancelEvent> CombatAnimationEditor::LoadCancelEvent(const nlohmann::json& eventJson) const
{
	std::shared_ptr<CancelEvent> cancelEvent = std::make_shared<CancelEvent>();
	cancelEvent->eventType = EventType::kCancel;
	cancelEvent->startEventTime = eventJson["StartEventTime"];
	cancelEvent->endEventTime = eventJson["EndEventTime"];
	cancelEvent->cancelType = eventJson["CancelType"];
	return cancelEvent;
}

std::shared_ptr<BufferedActionEvent> CombatAnimationEditor::LoadBufferedActionEvent(const nlohmann::json& eventJson) const
{
	std::shared_ptr<BufferedActionEvent> bufferedActionEvent = std::make_shared<BufferedActionEvent>();
	bufferedActionEvent->eventType = EventType::kBufferedAction;
	bufferedActionEvent->startEventTime = eventJson["StartEventTime"];
	bufferedActionEvent->endEventTime = eventJson["EndEventTime"];
	bufferedActionEvent->bufferedActionType = eventJson["BufferedActionType"];
	return bufferedActionEvent;
}

void CombatAnimationEditor::ManageAnimationSpeedConfigs(std::vector<AnimationSpeedConfig>& animationSpeedConfigs)
{
	//アニメーション速度の設定を追加
	ImGui::SeparatorText("Animation Speed Configs");
	AddAnimationSpeedConfig(animationSpeedConfigs);

	//アニメーション速度の設定が存在しない場合何もしない
	if (animationSpeedConfigs.empty()) { return; };

	//アニメーション速度の設定を編集
	EditAnimationSpeedConfigs(animationSpeedConfigs);
}

void CombatAnimationEditor::AddAnimationSpeedConfig(std::vector<AnimationSpeedConfig>& animationSpeedConfigs)
{
	//アニメーション速度の設定を追加
	if (ImGui::Button("Add Animation Speed Config"))
	{
		//アニメーション速度の設定を追加
		animationSpeedConfigs.push_back(AnimationSpeedConfig());
	}
}

void CombatAnimationEditor::EditAnimationSpeedConfigs(std::vector<AnimationSpeedConfig>& animationSpeedConfigs)
{
	//編集中かどうかのフラグ
	bool isAdjusting = false;

	//アニメーション速度設定のすべてを編集
	for (int32_t i = 0; i < animationSpeedConfigs.size(); ++i)
	{
		//ツリーノードの名前を設定
		std::string nodeName = "Animation Speed Config " + std::to_string(i);

		//ツリーノードが展開されなければ次のアイテムに進む
		if (!ImGui::TreeNode(nodeName.c_str()))
		{
			continue;
		}

		//持続時間を設定
		ImGui::DragFloat("Duration", &animationSpeedConfigs[i].duration, 0.001f);

		//調整中の場合はフラグを立てる
		if (ImGui::IsItemActive())
		{
			isAdjusting = true;
		}

		//アニメーションの速度を設定
		ImGui::DragFloat("Animation Speed", &animationSpeedConfigs[i].animationSpeed, 0.001f);

		//削除ボタンが押された場合
		if (ImGui::Button("Delete"))
		{
			//アニメーション速度設定を削除
			animationSpeedConfigs.erase(animationSpeedConfigs.begin() + i);
			//ツリーノードを閉じる
			ImGui::TreePop();
			break;
		}

		//ツリーノードを閉じる
		ImGui::TreePop();
	}

	//調整終了していたらソートする
	if (!isAdjusting)
	{
		//アニメーション速度の設定をソート
		std::sort(animationSpeedConfigs.begin(), animationSpeedConfigs.end(), [](const AnimationSpeedConfig& config1, const AnimationSpeedConfig& config2) {
			return config1.duration < config2.duration; }
		);
	}
}

void CombatAnimationEditor::ManageAnimationEvents(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents)
{
	//アニメーションイベントを追加
	ImGui::SeparatorText("Add Animation Event");
	AddAnimationEvent(animationEvents);

	//アニメーションイベントが存在しない場合何もしない
	if (animationEvents.empty()) { return; };

	//アニメーションイベントを編集
	ImGui::SeparatorText("Animation Events");
	EditAnimationEvents(animationEvents);
}

void CombatAnimationEditor::AddAnimationEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents)
{
	//イベントタイプの選択
	static int selectedEventType = 0;
	ImGui::Combo("Event Type", &selectedEventType, EVENT_TYPES, IM_ARRAYSIZE(EVENT_TYPES));

	//アニメーションイベントを追加
	switch (static_cast<EventType>(selectedEventType))
	{
	case EventType::kMovement:
		//移動イベントの追加
		AddMovementEvent(animationEvents);
		break;
	case EventType::kRotation:
		//回転イベントの追加
		AddRotationEvent(animationEvents);
		break;
	case EventType::kAttack:
		//攻撃イベントの追加
		AddAttackEvent(animationEvents);
		break;
	case EventType::kCameraAnimation:
		//カメラアニメーションイベントの追加
		AddCameraAnimationEvent(animationEvents);
		break;
	case EventType::kCancel:
		//キャンセルイベントの追加
		AddCancelEvent(animationEvents);
		break;
	case EventType::kBufferedAction:
		//先行入力イベントの追加
		AddBufferedActionEvent(animationEvents);
		break;
	}
}

void CombatAnimationEditor::AddMovementEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents)
{
	//移動イベントの種類を選択
	static int selectedMovementType = 0;
	ImGui::Combo("Movement Type", &selectedMovementType, MOVEMENT_TYPES, IM_ARRAYSIZE(MOVEMENT_TYPES));

	//移動イベントを追加
	if (ImGui::Button("Add Movement Event"))
	{
		switch (static_cast<MovementType>(selectedMovementType))
		{
		case MovementType::kVelocity:
			//速度移動イベントを追加
			animationEvents.push_back(std::make_shared<VelocityMovementEvent>());
			break;
		case MovementType::kEasing:
			//イージング移動イベントを追加
			animationEvents.push_back(std::make_shared<EasingMovementEvent>());
			break;
		}
	}
}

void CombatAnimationEditor::AddRotationEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents)
{
	//回転イベントを追加
	if (ImGui::Button("Add Rotation Event"))
	{
		animationEvents.push_back(std::make_shared<RotationEvent>());
	}
}

void CombatAnimationEditor::AddAttackEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents)
{
	//攻撃イベントを追加
	if (ImGui::Button("Add Attack Event"))
	{
		animationEvents.push_back(std::make_shared<AttackEvent>());
	}
}

void CombatAnimationEditor::AddCameraAnimationEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents)
{
	//カメラアニメーションイベントを追加
	if (ImGui::Button("Add Camera Animation Event"))
	{
		animationEvents.push_back(std::make_shared<CameraAnimationEvent>());
	}
}

void CombatAnimationEditor::AddCancelEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents)
{
	//キャンセルイベントを追加
	if (ImGui::Button("Add Cancel Event"))
	{
		animationEvents.push_back(std::make_shared<CancelEvent>());
	}
}

void CombatAnimationEditor::AddBufferedActionEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents)
{
	//先行入力イベントを追加
	if (ImGui::Button("Add Buffered Action Event"))
	{
		animationEvents.push_back(std::make_shared<BufferedActionEvent>());
	}
}

void CombatAnimationEditor::EditAnimationEvents(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents)
{
	//アニメーションイベントの名前
	static const std::unordered_map<EventType, std::string> eventNames = {
		{ EventType::kMovement, "Movement Event" },
		{ EventType::kRotation, "Rotation Event"},
		{ EventType::kAttack, "Attack Event " },
		{ EventType::kCameraAnimation, "Camera Animation Event "},
		{ EventType::kCancel, "Cancel Event " },
		{ EventType::kBufferedAction, "Buffered Action Event " },
	};

	//アニメーションイベントのインデックス
	int32_t animationEventIndex[static_cast<int>(EventType::kMaxEvent)]{};

	//アニメーションイベントをソート
	std::sort(animationEvents.begin(), animationEvents.end(), [](const std::shared_ptr<AnimationEvent>& event1, const std::shared_ptr<AnimationEvent>& event2) {
		return static_cast<int>(event1->eventType) < static_cast<int>(event2->eventType); }
	);

	//全てのアニメーションイベントを編集
	for (int32_t i = 0; i < animationEvents.size(); ++i)
	{
		//ツリーノードの名前を設定
		auto it = eventNames.find(animationEvents[i]->eventType);
		std::string nodeName = it->second + std::to_string(animationEventIndex[static_cast<int>(animationEvents[i]->eventType)]++);

		//ツリーノードを展開
		if (ImGui::TreeNode(nodeName.c_str()))
		{
			//アニメーションのイベントの種類に応じて表示する項目を変える
			switch (static_cast<EventType>(animationEvents[i]->eventType))
			{
			case EventType::kMovement:
				EditMovementEvent(dynamic_cast<MovementEvent*>(animationEvents[i].get()));
				break;
			case EventType::kRotation:
				EditRotationEvent(dynamic_cast<RotationEvent*>(animationEvents[i].get()));
				break;
			case EventType::kAttack:
				EditAttackEvent(dynamic_cast<AttackEvent*>(animationEvents[i].get()));
				break;
			case EventType::kCameraAnimation:
				EditCameraAnimationEvent(dynamic_cast<CameraAnimationEvent*>(animationEvents[i].get()));
				break;
			case EventType::kCancel:
				EditCancelEvent(dynamic_cast<CancelEvent*>(animationEvents[i].get()));
				break;
			case EventType::kBufferedAction:
				EditBufferedActionEvent(dynamic_cast<BufferedActionEvent*>(animationEvents[i].get()));
				break;
			}

			//削除ボタンを追加
			if (ImGui::Button("Delete"))
			{
				//イベントを削除
				animationEvents.erase(animationEvents.begin() + i);
				//ツリーノードを閉じる
				ImGui::TreePop();
				break;
			}

			//ツリーノードを閉じる
			ImGui::TreePop();
		}
	}
}

void CombatAnimationEditor::EditMovementEvent(MovementEvent* movementEvent)
{
	//移動イベントのパラメーターを編集
	EditEventTime(movementEvent);
	ImGui::Checkbox("Use Stick Input", &movementEvent->useStickInput);
	ImGui::Checkbox("Is Proximity Stop Enabled", &movementEvent->isProximityStopEnabled);
	ImGui::Checkbox("Move Towards Enemy", &movementEvent->moveTowardsEnemy);
	ImGui::Checkbox("Rotate Towards Movement", &movementEvent->rotateTowardsMovement);

	//移動の種類に応じて編集内容を変える
	if (movementEvent->movementType == MovementType::kVelocity)
	{
		//速度移動イベントを編集
		VelocityMovementEvent* velocityMovementEvent = dynamic_cast<VelocityMovementEvent*>(movementEvent);
		ImGui::DragFloat3("Velocity", &velocityMovementEvent->velocity.x, 0.001f);
	}
	else
	{
		//イージング移動イベントを編集
		EasingMovementEvent* easingMovementEvent = dynamic_cast<EasingMovementEvent*>(movementEvent);
		int easingTypeIndex = static_cast<int>(easingMovementEvent->easingType);
		if (ImGui::Combo("Easing Type", &easingTypeIndex, EASING_TYPES, IM_ARRAYSIZE(EASING_TYPES)))
		{
			easingMovementEvent->easingType = static_cast<EasingType>(easingTypeIndex);
		}
		ImGui::DragFloat3("Target Position", &easingMovementEvent->targetPosition.x, 0.001f);
	}
}

void CombatAnimationEditor::EditRotationEvent(RotationEvent* rotationEvent)
{
	//回転イベントのパラメーターを編集
	EditEventTime(rotationEvent);
	ImGui::DragFloat3("RotationAxis", &rotationEvent->rotationAxis.x, 0.001f);
	ImGui::DragFloat("Rotation Angle", &rotationEvent->rotationAngle, 0.001f);
	int easingTypeIndex = static_cast<int>(rotationEvent->easingType);
	if (ImGui::Combo("Easing Type", &easingTypeIndex, EASING_TYPES, IM_ARRAYSIZE(EASING_TYPES)))
	{
		rotationEvent->easingType = static_cast<EasingType>(easingTypeIndex);
	}
}

void CombatAnimationEditor::EditCameraAnimationEvent(CameraAnimationEvent* cameraAnimationEvent)
{
	//カメラアニメーションイベントのパラメーターを編集
	EditEventTime(cameraAnimationEvent);
	SelectFromMap("Camera Animation Name", cameraAnimationEvent->animationName, cameraAnimationEditor_->GetCameraPaths(), false);
	ImGui::DragFloat("Animation Speed", &cameraAnimationEvent->animationSpeed, 0.001f);
	ImGui::Checkbox("Sync With Character Animation", &cameraAnimationEvent->syncWithCharacterAnimation);
	int triggerIndex = static_cast<int>(cameraAnimationEvent->trigger);
	if (ImGui::Combo("Camera Animation Trigger", &triggerIndex, CAMERA_ANIMATION_TRIGGERS, IM_ARRAYSIZE(CAMERA_ANIMATION_TRIGGERS)))
	{
		cameraAnimationEvent->trigger = static_cast<CameraAnimationTrigger>(triggerIndex);
	}
}

void CombatAnimationEditor::EditAttackEvent(AttackEvent* attackEvent)
{
	//イベント時間を編集
	EditEventTime(attackEvent);

	//攻撃パラメータの編集
	if (ImGui::TreeNode("Attack Parameters"))
	{
		ImGui::DragInt("Hit Count", &attackEvent->attackParameters.maxHitCount);
		ImGui::DragFloat("Hit Interval", &attackEvent->attackParameters.hitInterval, 0.001f);
		ImGui::DragFloat("Damage", &attackEvent->attackParameters.damage, 0.001f);
		ImGui::TreePop();
	}

	//ヒットボックスパラメーターの編集
	if (ImGui::TreeNode("Hitbox Parameters"))
	{
		ImGui::DragFloat3("Center", &attackEvent->hitboxParameters.center.x, 0.001f);
		ImGui::DragFloat3("Size", &attackEvent->hitboxParameters.size.x, 0.001f);
		ImGui::TreePop();
	}

	//ノックバックパラメーターの編集
	if (ImGui::TreeNode("Knockback Parameters"))
	{
		ImGui::DragFloat3("Velocity", &attackEvent->knockbackParameters.velocity.x, 0.001f);
		ImGui::DragFloat3("Acceleration", &attackEvent->knockbackParameters.acceleration.x, 0.001f);
		ImGui::TreePop();
	}

	//エフェクトのパラメーターの編集
	if (ImGui::TreeNode("Effect Configs"))
	{
		ImGui::DragFloat("Hit Stop Duration", &attackEvent->effectConfigs.hitStopDuration, 0.001f);
		ImGui::DragFloat("Camera Shake Duration", &attackEvent->effectConfigs.cameraShakeDuration, 0.001f);
		ImGui::DragFloat3("Camera Shake Intensity", &attackEvent->effectConfigs.cameraShakeIntensity.x, 0.001f);
		SelectFromMap("Particle Effect Name", attackEvent->effectConfigs.hitParticleName, particleEffectEditor_->GetParticleEffectConfigs(), false);

		//ヒットSEタイプの選択
		int hitSETypeIndex = static_cast<int>(attackEvent->effectConfigs.hitSEType);
		if (ImGui::Combo("Hit SE Type", &hitSETypeIndex, HIT_SE_TYPES, IM_ARRAYSIZE(HIT_SE_TYPES)))
		{
			//選択したヒットSEタイプを設定
			attackEvent->effectConfigs.hitSEType = static_cast<HitSEType>(hitSETypeIndex);
		}

		//反応タイプの選択
		int selectedReactionType = static_cast<int>(attackEvent->effectConfigs.reactionType);
		if (ImGui::Combo("Reaction Type", &selectedReactionType, REACTION_TYPES, IM_ARRAYSIZE(REACTION_TYPES)))
		{
			//選択した反応タイプを設定
			attackEvent->effectConfigs.reactionType = static_cast<ReactionType>(selectedReactionType);
		}
		ImGui::TreePop();
	}
}

void CombatAnimationEditor::EditCancelEvent(CancelEvent* cancelEvent)
{
	//パラメーターを編集
	ImGui::DragFloat("Start Event Time", &cancelEvent->startEventTime, 0.001f);
	ImGui::DragFloat("End Event Time", &cancelEvent->endEventTime, 0.001f);
	EditCombo("Cancel Type", cancelEvent->cancelType, CANCEL_TYPES, IM_ARRAYSIZE(CANCEL_TYPES));
}

void CombatAnimationEditor::EditBufferedActionEvent(BufferedActionEvent* bufferedActionEvent)
{
	//パラメーターを編集
	ImGui::DragFloat("Start Event Time", &bufferedActionEvent->startEventTime, 0.001f);
	ImGui::DragFloat("End Event Time", &bufferedActionEvent->endEventTime, 0.001f);
	EditCombo("Buffered Action Type", bufferedActionEvent->bufferedActionType, BUFFERED_ACTION_TYPES, IM_ARRAYSIZE(BUFFERED_ACTION_TYPES));
}