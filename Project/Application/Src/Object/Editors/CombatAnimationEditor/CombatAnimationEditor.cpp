/**
 * @file CombatAnimationEditor.cpp
 * @brief キャラクターのモーションを編集・管理するファイル
 * @author 青木智滉
 * @date
 */

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
	if (ImGui::BeginTabBar("キャラクター"))
	{
		//各キャラクターのアニメーション編集
		for (const auto& editableCharacter : characterDatas_)
		{
			//アイテムバーを表示
			if (ImGui::BeginTabItem(editableCharacter.first.c_str()))
			{
				//キャラクターのアニメーションを編集
				EditCharacterAnimations(editableCharacter.second.character);

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
	//キャラクターデータを追加
	const std::string& characterName = character->GetName();
	characterDatas_[characterName].character = character;
}

const AnimationController& CombatAnimationEditor::GetAnimationController(const std::string& characterName, const std::string& animationName) const
{
	//キャラクターアニメーションを探す
	auto characterAnimation = characterDatas_.find(characterName);
	//キャラクターアニメーションが見つかった場合
	if (characterAnimation != characterDatas_.end())
	{
		//アニメーションコントローラーを探す
		auto animationController = characterAnimation->second.animationData.animationControllers.find(animationName);
		//アニメーションコントローラーが見つかった場合はそれを返す
		if (animationController != characterAnimation->second.animationData.animationControllers.end())
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
	CharacterAnimationData& characterAnimationData = characterDatas_[character->GetName()].animationData;

	//キャラクターのアニメーターを取得
	AnimatorComponent* animator = character->GetComponent<AnimatorComponent>();

	//アニメーションを選択
	SelectFromMap("アニメーション一覧", characterAnimationData.selectedAnimation, animator->GetAnimations(), true);

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
	ImGui::DragFloat3("動かす軸", &animationController.inPlaceAxis.x, 1.0f, 0.0f, 1.0f);

	//選択した戦闘アニメーションのコントロールを表示
	DisplayAnimationControllerControls(character->GetName(), characterAnimationData.selectedAnimation);

	//アニメーション速度の設定を管理
	ManageAnimationSpeedConfigs(animationController.animationSpeedConfigs);

	//アニメーションイベントを管理
	ManageAnimationEvents(animationController.animationEvents, character);
}

void CombatAnimationEditor::ToggleDebugMode(BaseCharacter* character, CharacterAnimationData& characterAnimationData, bool isDebug)
{
	//デバッグフラグの切り替え
	if (ImGui::Checkbox("デバッグ状態にするかどうか", &isDebug))
	{
		isDebug ? character->StartDebugMode(characterAnimationData.selectedAnimation) : character->EndDebugMode();
	}
}

void CombatAnimationEditor::EditAnimationTime(AnimatorComponent* animator, const std::string& selectedAnimationName, const bool isDebug)
{
	//現在編集しているアニメーションの時間を編集
	static float currentEditAnimationTime;
	ImGui::SliderFloat("アニメーションの時間", &currentEditAnimationTime, 0.0f, animator->GetAnimationDuration(selectedAnimationName));

	//デバッグ中の場合はアニメーターにアニメーションの時間を設定
	if (isDebug)
	{
		animator->SetAnimationTime(selectedAnimationName, currentEditAnimationTime);
	}
}

void CombatAnimationEditor::DisplayAnimationControllerControls(const std::string& characterName, const std::string& animationControllerName)
{
	//保存
	if (ImGui::Button("保存"))
	{
		SaveFile(characterName, animationControllerName);
	}

	//次のアイテムを同じ行に配置
	ImGui::SameLine();

	//読み込み
	if (ImGui::Button("読み込み"))
	{
		LoadFile(characterName, animationControllerName);
	}
}

void CombatAnimationEditor::SaveFile(const std::string& characterName, const std::string& animationControllerName)
{
	//アニメーションコントローラーを取得
	const AnimationController& animationController = characterDatas_[characterName].animationData.animationControllers[animationControllerName];

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
		case EventType::kEffect:
			SaveEffectEvent(dynamic_cast<EffectEvent*>(animationEvents[i].get()), eventJson);
			break;
		case EventType::kCameraAnimation:
			SaveCameraAnimationEvent(dynamic_cast<CameraAnimationEvent*>(animationEvents[i].get()), eventJson);
			break;
		case EventType::kQTE:
			SaveQTE(dynamic_cast<QTE*>(animationEvents[i].get()), eventJson);
			break;
		case EventType::kCancel:
			SaveCancelEvent(dynamic_cast<CancelEvent*>(animationEvents[i].get()), eventJson);
			break;
		case EventType::kBufferedAction:
			SaveBufferedActionEvent(dynamic_cast<BufferedActionEvent*>(animationEvents[i].get()), eventJson);
			break;
		}

		//IDを決める
		std::string id = i < 10 ? "0" + std::to_string(i) : std::to_string(i);

		//コンバットアニメーションのjsonオブジェクトにアニメーションイベントを追加
		animationEventsJson["AnimationEvent" + id] = eventJson;
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

	//パラメーターを保存
	eventJson.update({ {"EventType", "Attack"},{"StartEventTime", attackEvent->startEventTime},{"EndEventTime", attackEvent->endEventTime},
		{"MaxHitCount", attackParams.maxHitCount},{"HitInterval", attackParams.hitInterval},{"Damage", attackParams.damage},{"ReactionType", REACTION_TYPES[static_cast<int>(knockbackParams.reactionType)]},
		{"HitboxCenter", {hitboxParams.center.x, hitboxParams.center.y, hitboxParams.center.z}},{"HitboxSize", {hitboxParams.size.x, hitboxParams.size.y, hitboxParams.size.z}},
		{"KnockbackVelocity", {knockbackParams.velocity.x, knockbackParams.velocity.y, knockbackParams.velocity.z}},{"KnockbackAcceleration", {knockbackParams.acceleration.x, knockbackParams.acceleration.y, knockbackParams.acceleration.z}} });
}

void CombatAnimationEditor::SaveEffectEvent(const EffectEvent* effectEvent, nlohmann::json& eventJson)
{
	//パラメーターを保存
	eventJson.update({ {"EventType", "Effect"},{"StartEventTime", effectEvent->startEventTime},{"EndEventTime", effectEvent->endEventTime},
		{"HitStopDuration", effectEvent->hitStopDuration},{"CameraShakeDuration", effectEvent->cameraShakeDuration},{"CameraShakeIntensity", {effectEvent->cameraShakeIntensity.x, effectEvent->cameraShakeIntensity.y, effectEvent->cameraShakeIntensity.z}},
		{"SoundEffectType", effectEvent->soundEffectType},{"ParticleEffectName", effectEvent->particleEffectName},{"PostEffectType", POST_EFFECT_TYPES[static_cast<int>(effectEvent->postEffectType)]},
		{"EventTrigger", EVENT_TRIGGERS[static_cast<int>(effectEvent->trigger)]} });
}

void CombatAnimationEditor::SaveCameraAnimationEvent(const CameraAnimationEvent* cameraAnimationEvent, nlohmann::json& eventJson)
{
	//パラメーターを保存
	eventJson.update({ {"EventType", "CameraAnimation"},{"StartEventTime", cameraAnimationEvent->startEventTime},{"EndEventTime", cameraAnimationEvent->endEventTime},
		{"AnimationName", cameraAnimationEvent->animationName},{"AnimationSpeed", cameraAnimationEvent->animationSpeed},{"SyncWithCharacterAnimation", cameraAnimationEvent->syncWithCharacterAnimation},
		{"EventTrigger", EVENT_TRIGGERS[static_cast<int>(cameraAnimationEvent->trigger)]} });
}

void CombatAnimationEditor::SaveQTE(const QTE* qte, nlohmann::json& eventJson)
{
	//パラメーターを保存
	eventJson.update({ {"EventType", "QTE"},{"StartEventTime", qte->startEventTime},{"EndEventTime", qte->endEventTime}, {"RequiredTime", qte->requiredTime},
		{"QteType", qte->qteType}, {"TimeScale",qte->timeScale}, {"EventTrigger", EVENT_TRIGGERS[static_cast<int>(qte->trigger)]} });
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
	AnimationController& animationController = characterDatas_[characterName].animationData.animationControllers[animationControllerName];

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
		//エフェクトイベントを読み込む
		else if (eventType == "Effect")
		{
			animationEvent = LoadEffectEvent(eventItem);
		}
		//カメラアニメーションイベントを読み込む
		else if (eventType == "CameraAnimation")
		{
			animationEvent = LoadCameraAnimationEvent(eventItem);
		}
		//QTEを読み込む
		else if (eventType == "QTE")
		{
			animationEvent = LoadQTE(eventItem);
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

std::shared_ptr<MovementEvent> CombatAnimationEditor::LoadMovementEvent(const nlohmann::json& eventJson)
{
	//移動タイプを取得
	std::string movementType = eventJson["MovementType"];

	//移動タイプに応じて読み込むパラメーターを変更
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
	//アニメーションイベントの種類を設定
	movementEvent->eventType = EventType::kMovement;
	//アニメーションイベントの開始時間を設定
	movementEvent->startEventTime = eventJson["StartEventTime"];
	//アニメーションイベントの終了時間を設定
	movementEvent->endEventTime = eventJson["EndEventTime"];
	//スティックの入力をするかを設定
	movementEvent->useStickInput = eventJson["UseStickInput"];
	//敵と接近した時に移動をやめるかを設定
	movementEvent->isProximityStopEnabled = eventJson["IsProximityStopEnabled"];
	//敵の方向に移動するかを設定
	movementEvent->moveTowardsEnemy = eventJson["MoveTowardsEnemy"];
	//移動方向に回転するかどうかを設定
	movementEvent->rotateTowardsMovement = eventJson["RotateTowardsMovement"];
}

std::shared_ptr<VelocityMovementEvent> CombatAnimationEditor::LoadVelocityMovementEvent(const nlohmann::json& eventJson)
{
	//速度移動イベントを生成
	std::shared_ptr<VelocityMovementEvent> velocityMovementEvent = std::make_shared<VelocityMovementEvent>();
	//移動イベントの基本情報を設定
	InitializeCommonMovementEvent(velocityMovementEvent, eventJson);
	//移動の種類を設定
	velocityMovementEvent->movementType = MovementType::kVelocity;
	//速度を設定
	velocityMovementEvent->velocity = { eventJson["Velocity"][0].get<float>(), eventJson["Velocity"][1].get<float>(), eventJson["Velocity"][2].get<float>() };
	//生成した速度移動イベントを返す
	return velocityMovementEvent;
}

std::shared_ptr<EasingMovementEvent> CombatAnimationEditor::LoadEasingMovementEvent(const nlohmann::json& eventJson)
{
	//イージング移動イベントを生成
	std::shared_ptr<EasingMovementEvent> easingMovementEvent = std::make_shared<EasingMovementEvent>();
	//移動イベントの基本情報を設定
	InitializeCommonMovementEvent(easingMovementEvent, eventJson);
	//移動の種類を設定
	easingMovementEvent->movementType = MovementType::kEasing;
	//目標座標を設定
	easingMovementEvent->targetPosition = { eventJson["TargetPosition"][0].get<float>(), eventJson["TargetPosition"][1].get<float>(), eventJson["TargetPosition"][2].get<float>() };
	//イージングの種類を設定
	SetEnumFromJson(eventJson["EasingType"], easingMovementEvent->easingType, EASING_TYPES, IM_ARRAYSIZE(EASING_TYPES));
	//生成したイージング移動イベントを返す
	return easingMovementEvent;
}

std::shared_ptr<RotationEvent> CombatAnimationEditor::LoadRotationEvent(const nlohmann::json& eventJson)
{
	//回転イベントを生成
	std::shared_ptr<RotationEvent> rotationEvent = std::make_shared<RotationEvent>();
	//アニメーションイベントの種類を設定
	rotationEvent->eventType = EventType::kRotation;
	//アニメーションイベントの開始時間を設定
	rotationEvent->startEventTime = eventJson["StartEventTime"];
	//アニメーションイベントの終了時間を設定
	rotationEvent->endEventTime = eventJson["EndEventTime"];
	//総合回転量を設定
	rotationEvent->rotationAngle = eventJson["RotationAngle"];
	//回転軸を設定
	rotationEvent->rotationAxis = { eventJson["RotationAxis"][0].get<float>(), eventJson["RotationAxis"][1].get<float>(), eventJson["RotationAxis"][2].get<float>() };
	//イージングの種類を設定
	SetEnumFromJson(eventJson["EasingType"], rotationEvent->easingType, EASING_TYPES, IM_ARRAYSIZE(EASING_TYPES));
	//生成した回転イベントを返す
	return rotationEvent;
}

std::shared_ptr<AttackEvent> CombatAnimationEditor::LoadAttackEvent(const nlohmann::json& eventJson)
{
	//攻撃イベントを生成
	std::shared_ptr<AttackEvent> attackEvent = std::make_shared<AttackEvent>();
	//アニメーションイベントの種類を設定
	attackEvent->eventType = EventType::kAttack;
	//アニメーションイベントの開始時間を設定
	attackEvent->startEventTime = eventJson["StartEventTime"];
	//アニメーションイベントの終了時間を設定
	attackEvent->endEventTime = eventJson["EndEventTime"];
	//最大ヒット数を設定
	attackEvent->attackParameters.maxHitCount = eventJson["MaxHitCount"];
	//ヒット間隔を設定
	attackEvent->attackParameters.hitInterval = eventJson["HitInterval"];
	//ダメージを設定
	attackEvent->attackParameters.damage = eventJson["Damage"];
	//ヒットボックスの中心を設定
	attackEvent->hitboxParameters.center = { eventJson["HitboxCenter"][0].get<float>(), eventJson["HitboxCenter"][1].get<float>(), eventJson["HitboxCenter"][2].get<float>() };
	//ヒットボックスのサイズを設定
	attackEvent->hitboxParameters.size = { eventJson["HitboxSize"][0].get<float>(), eventJson["HitboxSize"][1].get<float>(), eventJson["HitboxSize"][2].get<float>() };
	//ノックバックの速度を設定
	attackEvent->knockbackParameters.velocity = { eventJson["KnockbackVelocity"][0].get<float>(), eventJson["KnockbackVelocity"][1].get<float>(), eventJson["KnockbackVelocity"][2].get<float>() };
	//ノックバックの加速度を設定
	attackEvent->knockbackParameters.acceleration = { eventJson["KnockbackAcceleration"][0].get<float>(), eventJson["KnockbackAcceleration"][1].get<float>(), eventJson["KnockbackAcceleration"][2].get<float>() };
	//リアクションの種類を設定
	SetEnumFromJson(eventJson["ReactionType"], attackEvent->knockbackParameters.reactionType, REACTION_TYPES, IM_ARRAYSIZE(REACTION_TYPES));
	//生成した攻撃イベントを返す
	return attackEvent;
}

std::shared_ptr<EffectEvent> CombatAnimationEditor::LoadEffectEvent(const nlohmann::json& eventJson)
{
	//エフェクトイベントを生成
	std::shared_ptr<EffectEvent> effectEvent = std::make_shared<EffectEvent>();
	//アニメーションイベントの種類を設定
	effectEvent->eventType = EventType::kEffect;
	//アニメーションイベントの開始時間を設定
	effectEvent->startEventTime = eventJson["StartEventTime"];
	//アニメーションイベントの終了時間を設定
	effectEvent->endEventTime = eventJson["EndEventTime"];
	//ヒットストップの持続時間を設定
	effectEvent->hitStopDuration = eventJson["HitStopDuration"];
	//カメラシェイクの持続時間を設定
	effectEvent->cameraShakeDuration = eventJson["CameraShakeDuration"];
	//カメラシェイクの強さを設定
	effectEvent->cameraShakeIntensity = { eventJson["CameraShakeIntensity"][0].get<float>(), eventJson["CameraShakeIntensity"][1].get<float>(), eventJson["CameraShakeIntensity"][2].get<float>() };
	//サウンドエフェクトの種類を設定
	effectEvent->soundEffectType = eventJson["SoundEffectType"];
	//パーティクルエフェクトの名前を設定
	effectEvent->particleEffectName = eventJson["ParticleEffectName"];
	//ポストエフェクトの種類を設定
	SetEnumFromJson(eventJson["PostEffectType"], effectEvent->postEffectType, POST_EFFECT_TYPES, IM_ARRAYSIZE(POST_EFFECT_TYPES));
	//イベントのトリガー条件を設定
	SetEnumFromJson(eventJson["EventTrigger"], effectEvent->trigger, EVENT_TRIGGERS, IM_ARRAYSIZE(EVENT_TRIGGERS));
	//生成したエフェクトイベントを返す
	return effectEvent;
}

std::shared_ptr<CameraAnimationEvent> CombatAnimationEditor::LoadCameraAnimationEvent(const nlohmann::json& eventJson)
{
	//カメラアニメーションイベントを生成
	std::shared_ptr<CameraAnimationEvent> cameraAnimationEvent = std::make_shared<CameraAnimationEvent>();
	//アニメーションイベントの種類を設定
	cameraAnimationEvent->eventType = EventType::kCameraAnimation;
	//アニメーションイベントの開始時間を設定
	cameraAnimationEvent->startEventTime = eventJson["StartEventTime"];
	//アニメーションイベントの終了時間を設定
	cameraAnimationEvent->endEventTime = eventJson["EndEventTime"];
	//アニメーションの名前を設定
	cameraAnimationEvent->animationName = eventJson["AnimationName"];
	//アニメーションの速度を設定
	cameraAnimationEvent->animationSpeed = eventJson["AnimationSpeed"];
	//キャラクターのアニメーションに同期するかどうかを設定
	cameraAnimationEvent->syncWithCharacterAnimation = eventJson["SyncWithCharacterAnimation"];
	//イベントのトリガー条件を設定
	SetEnumFromJson(eventJson["EventTrigger"], cameraAnimationEvent->trigger, EVENT_TRIGGERS, IM_ARRAYSIZE(EVENT_TRIGGERS));
	//生成したカメラアニメーションイベントを返す
	return cameraAnimationEvent;
}

std::shared_ptr<QTE> CombatAnimationEditor::LoadQTE(const nlohmann::json& eventJson)
{
	//QTEを生成
	std::shared_ptr<QTE> qte = std::make_shared<QTE>();
	//イベントタイプを設定
	qte->eventType = EventType::kQTE;
	//アニメーションイベントの開始時間を設定
	qte->startEventTime = eventJson["StartEventTime"];
	//アニメーションイベントの終了時間を設定
	qte->endEventTime = eventJson["EndEventTime"];
	//QTEの受付時間を設定
	qte->requiredTime = eventJson["RequiredTime"];
	//QTEの種類を設定
	qte->qteType = eventJson["QteType"];
	//タイムスケールを設定
	qte->timeScale = eventJson["TimeScale"];
	//イベントのトリガー条件を設定
	SetEnumFromJson(eventJson["EventTrigger"], qte->trigger, EVENT_TRIGGERS, IM_ARRAYSIZE(EVENT_TRIGGERS));
	//生成したQTEを返す
	return qte;
}

std::shared_ptr<CancelEvent> CombatAnimationEditor::LoadCancelEvent(const nlohmann::json& eventJson)
{
	//キャンセルイベントを生成
	std::shared_ptr<CancelEvent> cancelEvent = std::make_shared<CancelEvent>();
	//イベントタイプを設定
	cancelEvent->eventType = EventType::kCancel;
	//アニメーションイベントの開始時間を設定
	cancelEvent->startEventTime = eventJson["StartEventTime"];
	//アニメーションイベントの終了時間を設定
	cancelEvent->endEventTime = eventJson["EndEventTime"];
	//キャンセルアクションの種類を設定
	cancelEvent->cancelType = eventJson["CancelType"];
	//生成したキャンセルイベントを返す
	return cancelEvent;
}

std::shared_ptr<BufferedActionEvent> CombatAnimationEditor::LoadBufferedActionEvent(const nlohmann::json& eventJson)
{
	//先行入力イベントを生成
	std::shared_ptr<BufferedActionEvent> bufferedActionEvent = std::make_shared<BufferedActionEvent>();
	//イベントタイプを設定
	bufferedActionEvent->eventType = EventType::kBufferedAction;
	//アニメーションイベントの開始時間を設定
	bufferedActionEvent->startEventTime = eventJson["StartEventTime"];
	//アニメーションイベントの終了時間を設定
	bufferedActionEvent->endEventTime = eventJson["EndEventTime"];
	//先行入力の種類を設定
	bufferedActionEvent->bufferedActionType = eventJson["BufferedActionType"];
	//生成した先行入力イベントを返す
	return bufferedActionEvent;
}

void CombatAnimationEditor::ManageAnimationSpeedConfigs(std::vector<AnimationSpeedConfig>& animationSpeedConfigs)
{
	//アニメーション速度の設定を追加
	ImGui::SeparatorText("アニメーション速度設定");
	AddAnimationSpeedConfig(animationSpeedConfigs);

	//アニメーション速度の設定が存在しない場合何もしない
	if (animationSpeedConfigs.empty()) { return; };

	//アニメーション速度の設定を編集
	EditAnimationSpeedConfigs(animationSpeedConfigs);
}

void CombatAnimationEditor::AddAnimationSpeedConfig(std::vector<AnimationSpeedConfig>& animationSpeedConfigs)
{
	//アニメーション速度の設定を追加
	if (ImGui::Button("アニメーション速度設定を追加"))
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
		std::string nodeName = "アニメーション速度設定 " + std::to_string(i);

		//ツリーノードが展開されなければ次のアイテムに進む
		if (!ImGui::TreeNode(nodeName.c_str()))
		{
			continue;
		}

		//持続時間を設定
		ImGui::DragFloat("持続時間", &animationSpeedConfigs[i].duration, 0.001f);

		//調整中の場合はフラグを立てる
		if (ImGui::IsItemActive())
		{
			isAdjusting = true;
		}

		//アニメーションの速度を設定
		ImGui::DragFloat("アニメーションの速度", &animationSpeedConfigs[i].animationSpeed, 0.001f);

		//削除ボタンが押された場合
		if (ImGui::Button("削除"))
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

void CombatAnimationEditor::ManageAnimationEvents(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents, const BaseCharacter* character)
{
	//アニメーションイベントを追加
	ImGui::SeparatorText("アニメーションイベントを追加");
	AddAnimationEvent(animationEvents);

	//アニメーションイベントが存在しない場合何もしない
	if (animationEvents.empty()) { return; };

	//アニメーションイベントを編集
	ImGui::SeparatorText("アニメーションイベントを編集");
	EditAnimationEvents(animationEvents, character);
}

void CombatAnimationEditor::AddAnimationEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents)
{
	//イベントタイプの選択
	static int selectedEventType = 0;
	ImGui::Combo("イベントの種類", &selectedEventType, EVENT_TYPES, IM_ARRAYSIZE(EVENT_TYPES));

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
	case EventType::kEffect:
		//エフェクトイベントの追加
		AddEffectEvent(animationEvents);
		break;
	case EventType::kCameraAnimation:
		//カメラアニメーションイベントの追加
		AddCameraAnimationEvent(animationEvents);
		break;
	case EventType::kQTE:
		//QTEの追加
		AddQTE(animationEvents);
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
	ImGui::Combo("移動イベントの種類", &selectedMovementType, MOVEMENT_TYPES, IM_ARRAYSIZE(MOVEMENT_TYPES));

	//移動イベントを追加
	if (ImGui::Button("移動イベントを追加"))
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
	if (ImGui::Button("回転イベントを追加"))
	{
		animationEvents.push_back(std::make_shared<RotationEvent>());
	}
}

void CombatAnimationEditor::AddAttackEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents)
{
	//攻撃イベントを追加
	if (ImGui::Button("攻撃イベントを追加"))
	{
		animationEvents.push_back(std::make_shared<AttackEvent>());
	}
}

void CombatAnimationEditor::AddEffectEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents)
{
	//エフェクトイベントを追加
	if (ImGui::Button("エフェクトイベントを追加"))
	{
		animationEvents.push_back(std::make_shared<EffectEvent>());
	}
}

void CombatAnimationEditor::AddCameraAnimationEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents)
{
	//カメラアニメーションイベントを追加
	if (ImGui::Button("カメラアニメーションイベントを追加"))
	{
		animationEvents.push_back(std::make_shared<CameraAnimationEvent>());
	}
}

void CombatAnimationEditor::AddQTE(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents)
{
	//QTEを追加
	if (ImGui::Button("QTEを追加"))
	{
		animationEvents.push_back(std::make_shared<QTE>());
	}
}

void CombatAnimationEditor::AddCancelEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents)
{
	//キャンセルイベントを追加
	if (ImGui::Button("キャンセルイベントを追加"))
	{
		animationEvents.push_back(std::make_shared<CancelEvent>());
	}
}

void CombatAnimationEditor::AddBufferedActionEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents)
{
	//先行入力イベントを追加
	if (ImGui::Button("先行入力イベントを追加"))
	{
		animationEvents.push_back(std::make_shared<BufferedActionEvent>());
	}
}

void CombatAnimationEditor::EditAnimationEvents(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents, const BaseCharacter* character)
{
	//アニメーションイベントの名前
	static const std::unordered_map<EventType, std::string> eventNames = { { EventType::kMovement, "移動イベント " }, { EventType::kRotation, "回転イベント "}, { EventType::kAttack, "攻撃イベント " },
		{ EventType::kEffect, "エフェクトイベント "},{ EventType::kCameraAnimation, "カメラアニメーションイベント "}, { EventType::kQTE, "QTE "}, { EventType::kCancel, "キャンセルイベント " }, { EventType::kBufferedAction, "先行入力イベント " }
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
			case EventType::kEffect:
				EditEffectEvent(dynamic_cast<EffectEvent*>(animationEvents[i].get()), character->GetAudioHandles());
				break;
			case EventType::kCameraAnimation:
				EditCameraAnimationEvent(dynamic_cast<CameraAnimationEvent*>(animationEvents[i].get()));
				break;
			case EventType::kQTE:
				EditQTE(dynamic_cast<QTE*>(animationEvents[i].get()), character->GetQTEActions());
				break;
			case EventType::kCancel:
				EditCancelEvent(dynamic_cast<CancelEvent*>(animationEvents[i].get()), character->GetActionKeys());
				break;
			case EventType::kBufferedAction:
				EditBufferedActionEvent(dynamic_cast<BufferedActionEvent*>(animationEvents[i].get()), character->GetActionKeys());
				break;
			}

			//削除ボタンを追加
			if (ImGui::Button("削除"))
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
	//アニメーションイベントの時間を編集
	EditEventTime(movementEvent);

	//スティック入力を受け付けるかを設定
	ImGui::Checkbox("スティック入力を受け付けるかどうか", &movementEvent->useStickInput);

	//敵に近づいたら移動をやめるかを設定
	ImGui::Checkbox("敵に近づいたら移動をやめるかどうか", &movementEvent->isProximityStopEnabled);

	//敵の方向に進むかを設定
	ImGui::Checkbox("敵の方向に進むかどうか", &movementEvent->moveTowardsEnemy);

	//進行方向に回転するかどうかを設定
	ImGui::Checkbox("進行方向に回転させるかどうか", &movementEvent->rotateTowardsMovement);

	//移動の種類に応じて編集内容を変える
	if (movementEvent->movementType == MovementType::kVelocity)
	{
		//速度移動イベントにキャスト
		VelocityMovementEvent* velocityMovementEvent = dynamic_cast<VelocityMovementEvent*>(movementEvent);

		//速度を編集
		ImGui::DragFloat3("速度", &velocityMovementEvent->velocity.x, 0.001f);
	}
	else
	{
		//イージング移動イベントにキャスト
		EasingMovementEvent* easingMovementEvent = dynamic_cast<EasingMovementEvent*>(movementEvent);

		//目標座標を編集
		ImGui::DragFloat3("目標座標", &easingMovementEvent->targetPosition.x, 0.001f);

		//イージングの種類を選択
		SelectFromEnum("イージングの種類", easingMovementEvent->easingType, EASING_TYPES, IM_ARRAYSIZE(EASING_TYPES));
	}
}

void CombatAnimationEditor::EditRotationEvent(RotationEvent* rotationEvent)
{
	//アニメーションイベントの時間を編集
	EditEventTime(rotationEvent);

	//回転軸を編集
	ImGui::DragFloat3("回転軸", &rotationEvent->rotationAxis.x, 0.001f);

	//総合回転量を編集
	ImGui::DragFloat("回転量", &rotationEvent->rotationAngle, 0.001f);

	//イージングの種類を選択
	SelectFromEnum("イージングの種類", rotationEvent->easingType, EASING_TYPES, IM_ARRAYSIZE(EASING_TYPES));
}

void CombatAnimationEditor::EditAttackEvent(AttackEvent* attackEvent)
{
	//アニメーションイベントの時間を編集
	EditEventTime(attackEvent);

	//攻撃パラメータの編集
	if (ImGui::TreeNode("攻撃パラメーター"))
	{
		//最大ヒット数を編集
		ImGui::DragInt("最大ヒット数", &attackEvent->attackParameters.maxHitCount);

		//ヒット間隔を編集
		ImGui::DragFloat("ヒット間隔", &attackEvent->attackParameters.hitInterval, 0.001f);

		//ダメージを編集
		ImGui::DragFloat("ダメージ", &attackEvent->attackParameters.damage, 0.001f);

		//ツリーノードを閉じる
		ImGui::TreePop();
	}

	//ヒットボックスパラメーターの編集
	if (ImGui::TreeNode("ヒットボックスパラメーター"))
	{
		//ヒットボックスの中心を編集
		ImGui::DragFloat3("ヒットボックスの中心", &attackEvent->hitboxParameters.center.x, 0.001f);

		//ヒットボックスのサイズを編集
		ImGui::DragFloat3("ヒットボックスのサイズ", &attackEvent->hitboxParameters.size.x, 0.001f);

		//ツリーノードを閉じる
		ImGui::TreePop();
	}

	//ノックバックパラメーターの編集
	if (ImGui::TreeNode("ノックバックパラメーター"))
	{
		//ノックバックの速度を編集
		ImGui::DragFloat3("ノックバックの速度", &attackEvent->knockbackParameters.velocity.x, 0.001f);

		//ノックバックの加速度を編集
		ImGui::DragFloat3("ノックバックの加速度", &attackEvent->knockbackParameters.acceleration.x, 0.001f);

		//リアクションのタイプを選択
		SelectFromEnum("リアクションのタイプ", attackEvent->knockbackParameters.reactionType, REACTION_TYPES, IM_ARRAYSIZE(REACTION_TYPES));

		//ツリーノードを閉じる
		ImGui::TreePop();
	}
}

void CombatAnimationEditor::EditEffectEvent(EffectEvent* effectEvent, const std::vector<std::string>& soundEffects)
{
	//アニメーションイベントの時間を編集
	EditEventTime(effectEvent);

	//ヒットストップの持続時間を編集
	ImGui::DragFloat("ヒットストップの持続時間", &effectEvent->hitStopDuration, 0.001f);

	//カメラシェイクの持続時間を編集
	ImGui::DragFloat("カメラシェイクの持続時間", &effectEvent->cameraShakeDuration, 0.001f);

	//カメラシェイクの強さを編集
	ImGui::DragFloat3("カメラシェイクの強さ", &effectEvent->cameraShakeIntensity.x, 0.001f);

	//パーティクルエフェクトを選択
	SelectFromMap("パーティクルエフェクト", effectEvent->particleEffectName, particleEffectEditor_->GetParticleEffectConfigs(), false);

	//サウンドエフェクトタイプの選択
	EditCombo("サウンドエフェクト", effectEvent->soundEffectType, soundEffects);

	//ポストエフェクトのタイプを選択
	SelectFromEnum("ポストエフェクト", effectEvent->postEffectType, POST_EFFECT_TYPES, IM_ARRAYSIZE(POST_EFFECT_TYPES));

	//イベントのトリガー条件を選択
	SelectFromEnum("イベントの発生条件", effectEvent->trigger, EVENT_TRIGGERS, IM_ARRAYSIZE(EVENT_TRIGGERS));
}

void CombatAnimationEditor::EditCameraAnimationEvent(CameraAnimationEvent* cameraAnimationEvent)
{
	//アニメーションイベントの時間を編集
	EditEventTime(cameraAnimationEvent);

	//カメラアニメーションの種類を選択
	SelectFromMap("カメラアニメーションの種類", cameraAnimationEvent->animationName, cameraAnimationEditor_->GetCameraPaths(), false);

	//カメラアニメーションの速度を編集
	ImGui::DragFloat("カメラアニメーションの速度", &cameraAnimationEvent->animationSpeed, 0.001f);

	//キャラクターのアニメーションに同期するかどうかを設定
	ImGui::Checkbox("キャラクターのアニメーションに同期するか", &cameraAnimationEvent->syncWithCharacterAnimation);

	//イベントのトリガー条件を選択
	SelectFromEnum("イベントの発生条件", cameraAnimationEvent->trigger, EVENT_TRIGGERS, IM_ARRAYSIZE(EVENT_TRIGGERS));
}

void CombatAnimationEditor::EditCancelEvent(CancelEvent* cancelEvent, const std::vector<std::string>& actions)
{
	//アニメーションイベントの時間を編集
	EditEventTime(cancelEvent);

	//キャンセルアクションの種類を選択
	EditCombo("キャンセルアクションの種類", cancelEvent->cancelType, actions);
}

void CombatAnimationEditor::EditBufferedActionEvent(BufferedActionEvent* bufferedActionEvent, const std::vector<std::string>& actions)
{
	//アニメーションイベントの時間を編集
	EditEventTime(bufferedActionEvent);

	//先行入力の種類を選択
	EditCombo("先行入力の種類", bufferedActionEvent->bufferedActionType, actions);
}

void CombatAnimationEditor::EditQTE(QTE* qte, const std::vector<std::string>& actions)
{
	//アニメーションイベントの時間を編集
	EditEventTime(qte);

	//QTEの種類を選択
	EditCombo("QTEの種類", qte->qteType, actions);

	//QTE受付時間を編集
	ImGui::DragFloat("QTEの受付時間", &qte->requiredTime, 0.01f);

	//タイムスケールを編集
	ImGui::DragFloat("タイムスケール", &qte->timeScale, 0.001f);

	//イベントのトリガー条件を選択
	SelectFromEnum("イベントの発生条件", qte->trigger, EVENT_TRIGGERS, IM_ARRAYSIZE(EVENT_TRIGGERS));
}

void CombatAnimationEditor::EditCombo(const char* label, std::string& selectedName, const std::vector<std::string>& items)
{
	//コンボボックスを作成
	if (ImGui::BeginCombo(label, selectedName.c_str()))
	{
		//全てのアイテムを探す
		for (int i = 0; i < items.size(); ++i)
		{
			//選択されたかどうかのフラグ
			bool isSelected = (selectedName == items[i]);
			//項目を追加
			if (ImGui::Selectable(items[i].c_str(), isSelected))
			{
				//選択された値を更新
				selectedName = items[i];
			}

			//初めて選択された項目にチェックマークを表示
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		//コンボボックスを終了
		ImGui::EndCombo();
	}
}