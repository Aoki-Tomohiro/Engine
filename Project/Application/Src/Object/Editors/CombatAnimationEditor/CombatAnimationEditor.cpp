#include "CombatAnimationEditor.h"
#include "Application/Src/Object/Character/BaseCharacter.h"

namespace {
	const char* EVENT_TYPES[] = { "Movement", "Attack", "Cancel", "BufferedAction"};      //イベントタイプ
	const char* MOVEMENT_TYPES[] = { "AddVelocity", "Easing" };                           //移動イベントタイプ
	const char* HIT_SE_TYPES[] = { "Normal" };                                            //ヒット音SEタイプ
	const char* REACTION_TYPES[] = { "Flinch", "Knockback" };                             //反応タイプ
	const char* CANCEL_TYPES[] = { "Move", "Dodge", "Dash", "Attack", "Skill" };          //キャンセルタイプ
	const char* BUFFERED_ACTION_TYPES[] = { "Move", "Dodge", "Dash", "Attack", "Skill" }; //先行入力タイプ
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
			if (ImGui::BeginTabItem(editableCharacter.first.c_str()))
			{
				EditCharacterAnimations(editableCharacter.second);
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
	if (ImGui::Checkbox("IsDebug", &isDebug))
	{
		isDebug ? character->StartDebugMode(characterAnimationData.selectedAnimation) : character->EndDebugMode();
	}

	//アニメーションの時間を編集
	EditAnimationTime(animator, characterAnimationData.selectedAnimation, isDebug);

	//アニメーションコントローラーを取得
	AnimationController& animationController = characterAnimationData.animationControllers[characterAnimationData.selectedAnimation];

	//選択した戦闘アニメーションのコントロールを表示
	DisplayAnimationControllerControls(character->GetName(), characterAnimationData.selectedAnimation);

	//アニメーション速度の設定を取得
	std::vector<AnimationSpeedConfig>& animationSpeedConfigs = animationController.animationSpeedConfigs;

	//アニメーション速度の設定を管理
	ManageAnimationSpeedConfigs(animationSpeedConfigs);

	//全てのアニメーションイベントを取得
	std::vector<std::shared_ptr<AnimationEvent>>& animationEvents = animationController.animationEvents;

	//アニメーションイベントを管理
	ManageAnimationEvents(animationEvents);
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

	//rootのjsonオブジェクトを作成
	nlohmann::json root = nlohmann::json::object();

	//アニメーションコントローラーのjsonオブジェクトを作成
	nlohmann::json animationControllerJson = nlohmann::json::object();

	//アニメーション速度設定のjsonオブジェクトを作成
	nlohmann::json animationSpeedConfigsJson = nlohmann::json::object();

	//アニメーション速度の設定を保存
	SaveAnimationSpeedConfigs(animationController.animationSpeedConfigs, animationSpeedConfigsJson);

	//アニメーションコントローラーのjsonオブジェクトにアニメーション速度設定のjsonオブジェクトを保存
	animationControllerJson["AnimationSpeedConfigs"] = animationSpeedConfigsJson;

	//アニメーションイベントのjsonオブジェクトを作成
	nlohmann::json animationEventsJson = nlohmann::json::object();

	//アニメーションイベントを保存
	SaveAnimationEvents(animationController.animationEvents, animationEventsJson);

	//アニメーションコントローラーのjsonオブジェクトにアニメーションイベントのjsonオブジェクトを保存
	animationControllerJson["AnimationEvents"] = animationEventsJson;

	//rootオブジェクトにアニメーションコントローラーのjsonオブジェクトを追加
	root[characterName][animationControllerName] = animationControllerJson;

	//ディレクトリがなければ作成する
	std::string directoryPath = kDirectoryPath + characterName + "/";
	std::filesystem::path dir(directoryPath);
	if (!std::filesystem::exists(directoryPath))
	{
		std::filesystem::create_directory(directoryPath);
	}

	//書き込むjsonファイルのフルパスを合成する
	std::string filePath = directoryPath + animationControllerName + ".json";
	//書き込み用のファイルストリーム
	std::ofstream ofs;
	//ファイルを書き込み用に開く
	ofs.open(filePath);
	//ファイルオープン失敗
	if (ofs.fail())
	{
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "CombatAnimationEditor", 0);
		assert(0);
		return;
	}

	//ファイルにjson文字列を書き込む
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
		nlohmann::json animationSpeedJson = nlohmann::json::object();

		//持続時間を保存
		animationSpeedJson["Duration"] = animationSpeedConfigs[i].duration;

		//アニメーション速度を保存
		animationSpeedJson["AnimationSpeed"] = animationSpeedConfigs[i].animationSpeed;

		//アニメーション速度設定をjsonオブジェクトに書き込む
		animationSpeedConfigsJson["AnimationSpeedConfig" + std::to_string(i)] = animationSpeedJson;
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
		case EventType::kAttack:
			SaveAttackEvent(dynamic_cast<AttackEvent*>(animationEvents[i].get()), eventJson);
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
	eventJson["EventType"] = "Movement";

	//移動の種類ごとに保存する項目を設定
	switch (static_cast<MovementType>(movementEvent->movementType))
	{
	case MovementType::kVelocity:
		SaveVelocityMovementEvent(dynamic_cast<const VelocityMovementEvent*>(movementEvent), eventJson);
		break;
	case MovementType::kEasing:
		SaveEasingMovementEvent(dynamic_cast<const EasingMovementEvent*>(movementEvent), eventJson);
		break;
	}
}

void CombatAnimationEditor::SaveVelocityMovementEvent(const VelocityMovementEvent* velocityMovementEvent, nlohmann::json& eventJson)
{
	//パラメーターを保存
	eventJson["MovementType"] = "Velocity";
	eventJson["UseStickInput"] = velocityMovementEvent->useStickInput;
	eventJson["IsProximityStopEnabled"] = velocityMovementEvent->isProximityStopEnabled;
	eventJson["MoveTowardsEnemy"] = velocityMovementEvent->moveTowardsEnemy;
	eventJson["StartEventTime"] = velocityMovementEvent->startEventTime;
	eventJson["EndEventTime"] = velocityMovementEvent->endEventTime;
	eventJson["MoveSpeed"] = velocityMovementEvent->moveSpeed;
}

void CombatAnimationEditor::SaveEasingMovementEvent(const EasingMovementEvent* easingMovementEvent, nlohmann::json& eventJson)
{
	//パラメーターを保存
	eventJson["MovementType"] = "Easing";
	eventJson["UseStickInput"] = easingMovementEvent->useStickInput;
	eventJson["IsProximityStopEnabled"] = easingMovementEvent->isProximityStopEnabled;
	eventJson["MoveTowardsEnemy"] = easingMovementEvent->moveTowardsEnemy;
	eventJson["StartEventTime"] = easingMovementEvent->startEventTime;
	eventJson["EndEventTime"] = easingMovementEvent->endEventTime;
	eventJson["TargetPosition"] = nlohmann::json::array({ easingMovementEvent->targetPosition.x,easingMovementEvent->targetPosition.y,easingMovementEvent->targetPosition.z });
}

void CombatAnimationEditor::SaveAttackEvent(const AttackEvent* attackEvent, nlohmann::json& eventJson)
{
	//アニメーションイベントの基本情報を保存
	eventJson["EventType"] = "Attack";
	eventJson["StartEventTime"] = attackEvent->startEventTime;
	eventJson["EndEventTime"] = attackEvent->endEventTime;

	//攻撃パラメーターを保存
	const AttackParameters& attackParameters = attackEvent->attackParameters;
	eventJson["MaxHitCount"] = attackParameters.maxHitCount;
	eventJson["HitInterval"] = attackParameters.hitInterval;
	eventJson["Damage"] = attackParameters.damage;

	//ヒットボックスパラメーターを保存
	const HitboxParameters& hitboxParameters = attackEvent->hitboxParameters;
	eventJson["HitboxCenter"] = nlohmann::json::array({ hitboxParameters.center.x, hitboxParameters.center.y, hitboxParameters.center.z });
	eventJson["HitboxSize"] = nlohmann::json::array({ hitboxParameters.size.x, hitboxParameters.size.y, hitboxParameters.size.z });

	//ノックバックパラメーターを保存
	const KnockbackParameters& knockbackParameters = attackEvent->knockbackParameters;
	eventJson["KnockbackVelocity"] = nlohmann::json::array({ knockbackParameters.velocity.x, knockbackParameters.velocity.y, knockbackParameters.velocity.z });
	eventJson["KnockbackAcceleration"] = nlohmann::json::array({ knockbackParameters.acceleration.x, knockbackParameters.acceleration.y, knockbackParameters.acceleration.z });

	//エフェクトの設定を保存
	const HitEffectConfig& hitEffectConfig = attackEvent->effectConfigs;
	eventJson["HitStopDuration"] = hitEffectConfig.hitStopDuration;
	eventJson["CameraShakeDuration"] = hitEffectConfig.cameraShakeDuration;
	eventJson["CameraShakeIntensity"] = nlohmann::json::array({ hitEffectConfig.cameraShakeIntensity.x, hitEffectConfig.cameraShakeIntensity.y, hitEffectConfig.cameraShakeIntensity.z });
	eventJson["HitParticleName"] = hitEffectConfig.hitParticleName;
	switch (hitEffectConfig.hitSEType)
	{
	case HitSEType::kNormal: eventJson["HitSEType"] = "Normal"; 
		break;
	}
	switch (hitEffectConfig.reactionType)
	{
	case ReactionType::kFlinch: eventJson["ReactionType"] = "Flinch";
		break;
	case ReactionType::kKnockback: eventJson["ReactionType"] = "Knockback";
		break;
	}
}

void CombatAnimationEditor::SaveCancelEvent(const CancelEvent* cancelEvent, nlohmann::json& eventJson)
{
	//パラメーターを保存
	eventJson["EventType"] = "Cancel";
	eventJson["StartEventTime"] = cancelEvent->startEventTime;
	eventJson["EndEventTime"] = cancelEvent->endEventTime;
	eventJson["CancelType"] = cancelEvent->cancelType;
	eventJson["IsAnimationCorrectionActive"] = cancelEvent->isAnimationCorrectionActive;
}

void CombatAnimationEditor::SaveBufferedActionEvent(const BufferedActionEvent* bufferedActionEvent, nlohmann::json& eventJson)
{
	//パラメーターを保存
	eventJson["EventType"] = "BufferedAction";
	eventJson["StartEventTime"] = bufferedActionEvent->startEventTime;
	eventJson["EndEventTime"] = bufferedActionEvent->endEventTime;
	eventJson["BufferedActionType"] = bufferedActionEvent->bufferedActionType;
	eventJson["IsAnimationCorrectionActive"] = bufferedActionEvent->isAnimationCorrectionActive;
}

void CombatAnimationEditor::LoadFiles()
{
	//保存先ディレクトリパス
	std::filesystem::path dir(kDirectoryPath);
	//ディレクトリが無ければスキップする
	if (!std::filesystem::exists(kDirectoryPath))
	{
		return;
	}

	//ディレクトリを検索
	std::filesystem::recursive_directory_iterator dir_it(kDirectoryPath);
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
		LoadFile(filePath.parent_path().filename().string(), filePath.stem().string());
	}
}

void CombatAnimationEditor::LoadFile(const std::string& characterName, const std::string& animationControllerName)
{
	//読み込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + characterName + "/" + animationControllerName + ".json";
	//読み込み用のファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込みように開く
	ifs.open(filePath);
	//ファイルオープン失敗
	if (ifs.fail())
	{
		std::string message = "Failed open data file for read.";
		MessageBoxA(nullptr, message.c_str(), "CombatAnimationEditor", 0);
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

	//アニメーション速度設定を取得
	std::vector<AnimationSpeedConfig>& animationSpeedConfigs = animationController.animationSpeedConfigs;
	animationSpeedConfigs.clear();

	//アニメーション速度設定を読み込む
	LoadAnimationSpeedConfigs(animationController.animationSpeedConfigs, animationControllerJson["AnimationSpeedConfigs"]);

	//アニメーションイベントを取得
	std::vector<std::shared_ptr<AnimationEvent>>& animationEvents = animationController.animationEvents;
	animationEvents.clear();

	//アニメーションイベントを読み込む
	LoadAnimationEvents(animationEvents, animationControllerJson["AnimationEvents"]);
}

void CombatAnimationEditor::LoadAnimationSpeedConfigs(std::vector<AnimationSpeedConfig>& animationSpeedConfigs, nlohmann::json& animationSpeedConfigsJson)
{
	//全てのアニメーション速度設定を読み込む
	for (nlohmann::json::iterator animationSpeedConfigItem = animationSpeedConfigsJson.begin(); animationSpeedConfigItem != animationSpeedConfigsJson.end(); ++animationSpeedConfigItem)
	{
		//追加するアニメーション速度設定
		AnimationSpeedConfig animationSpeedConfig{};

		//アニメーション速度設定を取得
		nlohmann::json animationSpeedJson = animationSpeedConfigItem.value();

		//持続時間を取得
		animationSpeedConfig.duration = animationSpeedJson["Duration"];

		//アニメーションの速度を取得
		animationSpeedConfig.animationSpeed = animationSpeedJson["AnimationSpeed"];

		//アニメーション速度設定を追加
		animationSpeedConfigs.push_back(animationSpeedConfig);
	}
}

void CombatAnimationEditor::LoadAnimationEvents(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents, nlohmann::json& eventJson)
{
	//全てのアニメーションイベントを読み込む
	for (nlohmann::json::iterator eventItem = eventJson.begin(); eventItem != eventJson.end(); ++eventItem)
	{
		//アニメーションイベントのデータを取得
		nlohmann::json eventData = eventItem.value();

		//イベントタイプを取得
		std::string eventType = eventData["EventType"];

		//移動イベントを読み込む
		if (eventType == "Movement")
		{
			LoadMovementEvent(animationEvents, eventData);
		}
		//攻撃イベントを読み込む
		else if (eventType == "Attack")
		{
			LoadAttackEvent(animationEvents, eventData);
		}
		//キャンセルイベントを読み込む
		else if (eventType == "Cancel")
		{
			LoadCancelEvent(animationEvents, eventData);
		}
		//先行入力イベントを読み込む
		else if (eventType == "BufferedAction")
		{
			LoadBufferedActionEvent(animationEvents, eventData);
		}
	}
}

void CombatAnimationEditor::LoadMovementEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents, nlohmann::json& eventJson)
{
	//移動タイプを取得
	std::string movementType = eventJson["MovementType"];

	//移動タイプに応じて読み込むパラメーターを変更
	if (movementType == "Velocity")
	{
		LoadVelocityMovementEvent(animationEvents, eventJson);
	}
	else if (movementType == "Easing")
	{
		LoadEasingMovementEvent(animationEvents, eventJson);
	}
}

void CombatAnimationEditor::LoadVelocityMovementEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents, nlohmann::json& eventJson)
{
	VelocityMovementEvent* velocityMovementEvent = new VelocityMovementEvent();
	velocityMovementEvent->eventType = EventType::kMovement;
	velocityMovementEvent->movementType = MovementType::kVelocity;
	velocityMovementEvent->useStickInput = eventJson["UseStickInput"];
	velocityMovementEvent->isProximityStopEnabled = eventJson["IsProximityStopEnabled"];
	velocityMovementEvent->moveTowardsEnemy = eventJson["MoveTowardsEnemy"];
	velocityMovementEvent->startEventTime = eventJson["StartEventTime"];
	velocityMovementEvent->endEventTime = eventJson["EndEventTime"];
	velocityMovementEvent->moveSpeed = eventJson["MoveSpeed"];
	animationEvents.push_back(std::shared_ptr<VelocityMovementEvent>(velocityMovementEvent));
}

void CombatAnimationEditor::LoadEasingMovementEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents, nlohmann::json& eventJson)
{
	EasingMovementEvent* easingMovementEvent = new EasingMovementEvent();
	easingMovementEvent->eventType = EventType::kMovement;
	easingMovementEvent->movementType = MovementType::kEasing;
	easingMovementEvent->useStickInput = eventJson["UseStickInput"];
	easingMovementEvent->isProximityStopEnabled = eventJson["IsProximityStopEnabled"];
	easingMovementEvent->moveTowardsEnemy = eventJson["MoveTowardsEnemy"];
	easingMovementEvent->startEventTime = eventJson["StartEventTime"];
	easingMovementEvent->endEventTime = eventJson["EndEventTime"];
	easingMovementEvent->targetPosition = { eventJson["TargetPosition"][0].get<float>(), eventJson["TargetPosition"][1].get<float>(), eventJson["TargetPosition"][2].get<float>() };
	animationEvents.push_back(std::shared_ptr<EasingMovementEvent>(easingMovementEvent));
}

void CombatAnimationEditor::LoadAttackEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents, nlohmann::json& eventJson)
{
	AttackEvent* attackEvent = new AttackEvent();
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
	animationEvents.push_back(std::shared_ptr<AttackEvent>(attackEvent));
}

void CombatAnimationEditor::LoadCancelEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents, nlohmann::json& eventJson)
{
	CancelEvent* cancelEvent = new CancelEvent();
	cancelEvent->eventType = EventType::kCancel;
	cancelEvent->startEventTime = eventJson["StartEventTime"];
	cancelEvent->endEventTime = eventJson["EndEventTime"];
	cancelEvent->cancelType = eventJson["CancelType"];
	cancelEvent->isAnimationCorrectionActive = eventJson["IsAnimationCorrectionActive"];
	animationEvents.push_back(std::shared_ptr<CancelEvent>(cancelEvent));
}

void CombatAnimationEditor::LoadBufferedActionEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents, nlohmann::json& eventJson)
{
	BufferedActionEvent* bufferedActionEvent = new BufferedActionEvent();
	bufferedActionEvent->eventType = EventType::kBufferedAction;
	bufferedActionEvent->startEventTime = eventJson["StartEventTime"];
	bufferedActionEvent->endEventTime = eventJson["EndEventTime"];
	bufferedActionEvent->bufferedActionType = eventJson["BufferedActionType"];
	bufferedActionEvent->isAnimationCorrectionActive = eventJson["IsAnimationCorrectionActive"];
	animationEvents.push_back(std::shared_ptr<BufferedActionEvent>(bufferedActionEvent));
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
	//全てのアニメーション速度の設定を編集
	for (int32_t i = 0; i < animationSpeedConfigs.size(); ++i)
	{
		//ツリーノードの名前を設定
		std::string nodeName = "Animation Speed Config" + std::to_string(i);

		//ツリーノードを展開
		if (ImGui::TreeNode(nodeName.c_str()))
		{
			//持続時間を設定
			ImGui::DragFloat("Duration", &animationSpeedConfigs[i].duration, 0.001f);

			//アニメーションの速度を設定
			ImGui::DragFloat("AnimationSpeed", &animationSpeedConfigs[i].animationSpeed, 0.001f);

			//削除ボタン
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
	case EventType::kAttack:
		//攻撃イベントの追加
		AddAttackEvent(animationEvents);
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

void CombatAnimationEditor::EditVelocityMovementEvent(VelocityMovementEvent* velocityMovementEvent)
{
	//速度移動イベントのパラメーターを調整
	ImGui::DragFloat("Start Event Time", &velocityMovementEvent->startEventTime, 0.001f);
	ImGui::DragFloat("End Event Time", &velocityMovementEvent->endEventTime, 0.001f);
	ImGui::Checkbox("UseStickInput", &velocityMovementEvent->useStickInput);
	ImGui::Checkbox("IsProximityStopEnabled", &velocityMovementEvent->isProximityStopEnabled);
	ImGui::Checkbox("MoveTowardsEnemy", &velocityMovementEvent->moveTowardsEnemy);
	ImGui::DragFloat("MoveSpeed", &velocityMovementEvent->moveSpeed, 0.001f);
}

void CombatAnimationEditor::EditEasingMovementEvent(EasingMovementEvent* easingMovementEvent)
{
	//イージング移動イベントのパラメーターを調整
	ImGui::DragFloat("Start Event Time", &easingMovementEvent->startEventTime, 0.001f);
	ImGui::DragFloat("End Event Time", &easingMovementEvent->endEventTime, 0.001f);
	ImGui::Checkbox("UseStickInput", &easingMovementEvent->useStickInput);
	ImGui::Checkbox("IsProximityStopEnabled", &easingMovementEvent->isProximityStopEnabled);
	ImGui::Checkbox("MoveTowardsEnemy", &easingMovementEvent->moveTowardsEnemy);
	ImGui::DragFloat3("Target Position", &easingMovementEvent->targetPosition.x, 0.001f);
}

void CombatAnimationEditor::AddAttackEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents)
{
	//攻撃イベントを追加
	if (ImGui::Button("Add Attack Event"))
	{
		animationEvents.push_back(std::make_shared<AttackEvent>());
	}
}

void CombatAnimationEditor::EditAttackEvent(AttackEvent* attackEvent)
{
	//イベント時間を編集
	ImGui::DragFloat("Start Event Time", &attackEvent->startEventTime, 0.001f);
	ImGui::DragFloat("End Event Time", &attackEvent->endEventTime, 0.001f);

	//攻撃パラメータを編集するUI
	if (ImGui::TreeNode("Attack Parameters"))
	{
		ImGui::DragInt("Hit Count", &attackEvent->attackParameters.maxHitCount);
		ImGui::DragFloat("Hit Interval", &attackEvent->attackParameters.hitInterval, 0.001f);
		ImGui::DragFloat("Damage", &attackEvent->attackParameters.damage, 0.001f);
		ImGui::TreePop();
	}

	//ヒットボックスパラメータを編集するUI
	if (ImGui::TreeNode("Hitbox Parameters"))
	{
		ImGui::DragFloat3("Center", &attackEvent->hitboxParameters.center.x, 0.001f);
		ImGui::DragFloat3("Size", &attackEvent->hitboxParameters.size.x, 0.001f);
		ImGui::TreePop();
	}

	//ノックバックのパラメータを編集するUI
	if (ImGui::TreeNode("Knockback Parameters"))
	{
		ImGui::DragFloat3("Velocity", &attackEvent->knockbackParameters.velocity.x, 0.001f);
		ImGui::DragFloat3("Acceleration", &attackEvent->knockbackParameters.acceleration.x, 0.001f);
		ImGui::TreePop();
	}

	//効果設定のパラメータを編集するUI
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

void CombatAnimationEditor::AddCancelEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents)
{
	//キャンセルイベントを追加
	if (ImGui::Button("Add Cancel Event"))
	{
		animationEvents.push_back(std::make_shared<CancelEvent>());
	}
}

void CombatAnimationEditor::EditCancelEvent(CancelEvent* cancelEvent)
{
	//パラメーターを編集
	ImGui::DragFloat("Start Event Time", &cancelEvent->startEventTime, 0.001f);
	ImGui::DragFloat("End Event Time", &cancelEvent->endEventTime, 0.001f);
	ImGui::Checkbox("Is Animation Correction Active", &cancelEvent->isAnimationCorrectionActive);
	EditCombo("Cancel Type", cancelEvent->cancelType, CANCEL_TYPES, IM_ARRAYSIZE(CANCEL_TYPES));
}

void CombatAnimationEditor::AddBufferedActionEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents)
{
	//先行入力イベントを追加
	if (ImGui::Button("Add Buffered Action Event"))
	{
		animationEvents.push_back(std::make_shared<BufferedActionEvent>());
	}
}

void CombatAnimationEditor::EditBufferedActionEvent(BufferedActionEvent* bufferedActionEvent)
{
	//パラメーターを編集
	ImGui::DragFloat("Start Event Time", &bufferedActionEvent->startEventTime, 0.001f);
	ImGui::DragFloat("End Event Time", &bufferedActionEvent->endEventTime, 0.001f);
	ImGui::Checkbox("Is Animation Correction Active", &bufferedActionEvent->isAnimationCorrectionActive);
	EditCombo("Buffered Action Type", bufferedActionEvent->bufferedActionType, BUFFERED_ACTION_TYPES, IM_ARRAYSIZE(BUFFERED_ACTION_TYPES));
}

void CombatAnimationEditor::EditAnimationEvents(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents)
{
	//全てのアニメーションイベントを編集
	for (int32_t i = 0; i < animationEvents.size(); ++i)
	{
		//ツリーノードの名前を設定
		std::string nodeName = "Animation Event " + std::to_string(i);

		//ツリーノードを展開
		if (ImGui::TreeNode(nodeName.c_str()))
		{
			//アニメーションのイベントの種類に応じて表示する項目を変える
			switch (static_cast<EventType>(animationEvents[i]->eventType))
			{
			case EventType::kMovement:
				EditMovementEvent(dynamic_cast<MovementEvent*>(animationEvents[i].get()));
				break;
			case EventType::kAttack:
				EditAttackEvent(dynamic_cast<AttackEvent*>(animationEvents[i].get()));
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
	//移動の種類に応じて編集内容を変える
	switch (static_cast<MovementType>(movementEvent->movementType))
	{
	case MovementType::kVelocity:
		EditVelocityMovementEvent(dynamic_cast<VelocityMovementEvent*>(movementEvent));
		break;
	case MovementType::kEasing:
		EditEasingMovementEvent(dynamic_cast<EasingMovementEvent*>(movementEvent));
		break;
	}
}