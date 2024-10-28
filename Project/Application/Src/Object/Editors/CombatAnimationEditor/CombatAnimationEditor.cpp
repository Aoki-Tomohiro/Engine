#include "CombatAnimationEditor.h"

namespace {
	const char* EVENT_TYPES[] = { "Movement", "Attack" }; //イベントタイプ
	const char* MOVEMENT_TYPES[] = { "AddVelocity", "Easing" }; //移動イベントタイプ
	const char* HIT_SE_TYPES[] = { "Normal" }; //ヒット音SEタイプ
	const char* REACTION_TYPES[] = { "Flinch", "Knockback" }; //反応タイプ
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

void CombatAnimationEditor::EditCharacterAnimations(BaseCharacter* character)
{
	//キャラクターのアニメーションのデータを取得
	CharacterAnimationData& characterAnimationData = characterAnimations_[character->GetName()];

	//キャラクターのアニメーターを取得
	AnimatorComponent* animator = character->GetComponent<AnimatorComponent>();

	//アニメーションを選択
	SelectFromMap("Animations", characterAnimationData.selectedAnimationName, animator->GetAnimations(), true);

	//選択しているアニメーションがない場合は処理を飛ばす
	if (characterAnimationData.selectedAnimationName.empty()) { return; };

	//デバッグモードの設定
	bool isDebug = character->GetIsDebug();
	if (ImGui::Checkbox("IsDebug", &isDebug))
	{
		isDebug ? character->StartDebugMode(characterAnimationData.selectedAnimationName) : character->EndDebugMode();
	}

	//デバッグモード中は選択しているアニメーションの時間を編集
	if (isDebug)
	{
		EditAnimationTime(animator, characterAnimationData.selectedAnimationName);
	}

	//戦闘アニメーションの追加
	if (ImGui::Button("Add Combat Animation"))
	{
		AddCombatAnimation(characterAnimationData, characterAnimationData.selectedAnimationName);
	}

	//編集するアニメーションが存在しない場合は何もしない
	if (characterAnimationData.combatAnimations.empty()) { return; };

	//現在編集しているアニメーションの選択
	ImGui::SeparatorText("Edit Animation");
	SelectFromMap("Edit Animation", characterAnimationData.currentEditAnimationName, characterAnimationData.combatAnimations, false);

	//編集するアニメーションが選択されていない場合は何もしない
	if (characterAnimationData.currentEditAnimationName.empty()) { return; };

	//全てのアニメーションイベントを取得
	std::vector<std::unique_ptr<AnimationEvent>>& animationEvents = characterAnimationData.combatAnimations[characterAnimationData.currentEditAnimationName];

	//選択した戦闘アニメーションのコントロールを表示
	DisplayCombatAnimationControls(character->GetName(), characterAnimationData.currentEditAnimationName);

	//アニメーションイベントを追加
	ImGui::SeparatorText("Add Animation Event");
	AddAnimationEvent(animationEvents);

	//アニメーションイベントが存在しない場合何もしない
	if (animationEvents.empty()) { return; };

	//アニメーションイベントを編集
	ImGui::SeparatorText("Animation Events");
	EditAnimationEvents(animationEvents);
}

void CombatAnimationEditor::EditAnimationTime(AnimatorComponent* animator, const std::string& selectedAnimationName)
{
	//現在編集しているアニメーションの時間を編集
	static float currentEditAnimationTime;
	ImGui::SliderFloat("Animation Time", &currentEditAnimationTime, 0.0f, animator->GetAnimationDuration(selectedAnimationName));
	animator->SetAnimationTime(selectedAnimationName, currentEditAnimationTime);
}

void CombatAnimationEditor::AddCombatAnimation(CharacterAnimationData& characterAnimationData, const std::string& selectedAnimationName)
{
	//同じアニメーションイベントのコンテナが存在しなければ追加する
	if (characterAnimationData.combatAnimations.find(selectedAnimationName) == characterAnimationData.combatAnimations.end())
	{
		characterAnimationData.combatAnimations[selectedAnimationName] = std::vector<std::unique_ptr<AnimationEvent>>();
	}
}

void CombatAnimationEditor::DisplayCombatAnimationControls(const std::string& characterName, const std::string& combatAnimationName)
{
	//保存
	if (ImGui::Button("Save"))
	{
		SaveFile(characterName, combatAnimationName);
	}

	//次のアイテムを同じ行に配置
	ImGui::SameLine();

	//読み込み
	if (ImGui::Button("Load"))
	{
		LoadFile(characterName, combatAnimationName);
	}
}

void CombatAnimationEditor::SaveFile(const std::string& characterName, const std::string& combatAnimationName)
{
	//全てのアニメーションイベントを取得
	std::vector<std::unique_ptr<AnimationEvent>>& animationEvents = characterAnimations_[characterName].combatAnimations[combatAnimationName];

	//rootのjsonオブジェクトを作成
	nlohmann::json root = nlohmann::json::object();

	//コンバットアニメーションのjsonオブジェクトを作成
	nlohmann::json animationJson = nlohmann::json::object();

	//全てのアニメーションイベントの設定を保存
	for (int32_t i = 0; i < animationEvents.size(); ++i)
	{
		//アニメーションイベントのjsonオブジェクトを作成
		nlohmann::json eventJson = nlohmann::json::object();

		//アニメーションイベントごとに保存する項目を設定
		switch (static_cast<EventType>(animationEvents[i]->eventType))
		{
		case EventType::kMovement:
			SaveMovementEvent(dynamic_cast<MovementEvent*>(animationEvents[i].get()), eventJson);
			break;
		case EventType::kAttack:
			SaveAttackEvent(dynamic_cast<AttackEvent*>(animationEvents[i].get()), eventJson);
			break;
		}

		//コンバットアニメーションのjsonオブジェクトにアニメーションイベントを追加
		animationJson[animationEvents[i]->eventName] = eventJson;
	}

	//rootオブジェクトに追加
	root[characterName][combatAnimationName] = animationJson;

	//ディレクトリがなければ作成する
	std::string directoryPath = kDirectoryPath + characterName + "/";
	std::filesystem::path dir(directoryPath);
	if (!std::filesystem::exists(directoryPath))
	{
		std::filesystem::create_directory(directoryPath);
	}

	//書き込むjsonファイルのフルパスを合成する
	std::string filePath = directoryPath + combatAnimationName + ".json";
	//書き込み用のファイルストリーム
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

	//ファイルにjson文字列を書き込む
	ofs << std::setw(4) << root << std::endl;
	//ファイルを閉じる
	ofs.close();
}

void CombatAnimationEditor::SaveMovementEvent(MovementEvent* movementEvent, nlohmann::json& eventJson)
{
	//イベントのタイプを設定
	eventJson["EventType"] = "Movement";

	//移動の種類ごとに保存する項目を設定
	switch (static_cast<MovementType>(movementEvent->movementType))
	{
	case MovementType::kVelocity:
		SaveVelocityMovementEvent(dynamic_cast<VelocityMovementEvent*>(movementEvent), eventJson);
		break;
	case MovementType::kEasing:
		SaveEasingMovementEvent(dynamic_cast<EasingMovementEvent*>(movementEvent), eventJson);
		break;
	}
}

void CombatAnimationEditor::SaveVelocityMovementEvent(VelocityMovementEvent* velocityMovementEvent, nlohmann::json& eventJson)
{
	//パラメーターを保存
	eventJson["MovementType"] = "Velocity";
	eventJson["StartEventTime"] = velocityMovementEvent->startEventTime;
	eventJson["EndEventTime"] = velocityMovementEvent->endEventTime;
	eventJson["Velocity"] = nlohmann::json::array({ velocityMovementEvent->velocity.x,velocityMovementEvent->velocity.y,velocityMovementEvent->velocity.z });
}

void CombatAnimationEditor::SaveEasingMovementEvent(EasingMovementEvent* easingMovementEvent, nlohmann::json& eventJson)
{
	//パラメーターを保存
	eventJson["MovementType"] = "Easing";
	eventJson["StartEventTime"] = easingMovementEvent->startEventTime;
	eventJson["EndEventTime"] = easingMovementEvent->endEventTime;
	eventJson["TargetPosition"] = nlohmann::json::array({ easingMovementEvent->targetPosition.x,easingMovementEvent->targetPosition.y,easingMovementEvent->targetPosition.z });
}

void CombatAnimationEditor::SaveAttackEvent(AttackEvent* attackEvent, nlohmann::json& eventJson)
{
	//アニメーションイベントの基本情報を保存
	eventJson["EventType"] = "Attack";
	eventJson["StartEventTime"] = attackEvent->startEventTime;
	eventJson["EndEventTime"] = attackEvent->endEventTime;

	//攻撃パラメーターを保存
	AttackParameters& attackParameters = attackEvent->attackParameters;
	eventJson["HitCount"] = attackParameters.hitCount;
	eventJson["HitInterval"] = attackParameters.hitInterval;
	eventJson["Damage"] = attackParameters.damage;

	//ヒットボックスパラメーターを保存
	HitboxParameters& hitboxParameters = attackEvent->hitboxParameters;
	eventJson["HitboxCenter"] = nlohmann::json::array({ hitboxParameters.center.x, hitboxParameters.center.y, hitboxParameters.center.z });
	eventJson["HitboxSize"] = nlohmann::json::array({ hitboxParameters.size.x, hitboxParameters.size.y, hitboxParameters.size.z });

	//ノックバックパラメーターを保存
	KnockbackParameters& knockbackParameters = attackEvent->knockbackParameters;
	eventJson["KnockbackVelocity"] = nlohmann::json::array({ knockbackParameters.velocity.x, knockbackParameters.velocity.y, knockbackParameters.velocity.z });
	eventJson["KnockbackAcceleration"] = nlohmann::json::array({ knockbackParameters.acceleration.x, knockbackParameters.acceleration.y, knockbackParameters.acceleration.z });

	//エフェクトの設定を保存
	HitEffectConfig& hitEffectConfig = attackEvent->effectConfigs;
	eventJson["HitStopDuration"] = hitEffectConfig.hitStopDuration;
	eventJson["CameraShakeDuration"] = hitEffectConfig.cameraShakeDuration;
	eventJson["CameraShakeIntensity"] = nlohmann::json::array({ hitEffectConfig.cameraShakeIntensity.x, hitEffectConfig.cameraShakeIntensity.y, hitEffectConfig.cameraShakeIntensity.z });
	eventJson["HitParticleName"] = hitEffectConfig.hitParticleName;
	switch (static_cast<HitSEType>(hitEffectConfig.hitSEType))
	{
	case HitSEType::kNormal: eventJson["HitSEType"] = "Normal"; break;
	}
	switch (static_cast<ReactionType>(hitEffectConfig.reactionType))
	{
	case ReactionType::kFlinch: eventJson["ReactionType"] = "Flinch"; break;
	case ReactionType::kKnockback: eventJson["ReactionType"] = "Knockback"; break;
	}
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

void CombatAnimationEditor::LoadFile(const std::string& characterName, const std::string& combatAnimationName)
{
	//読み込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + characterName + "/" + combatAnimationName + ".json";
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

	//jsonに何も書かれていなければ処理を飛ばす
	if (root.empty()) { return; };

	//アニメーションイベントのデータを取得
	nlohmann::json animationJson = root[characterName][combatAnimationName];

	//全てアニメーションイベントを取得
	std::vector<std::unique_ptr<AnimationEvent>>& animationEvents = characterAnimations_[characterName].combatAnimations[combatAnimationName];
	animationEvents.clear();

	//各アニメーションイベントについて
	for (nlohmann::json::iterator animationItem = animationJson.begin(); animationItem != animationJson.end(); ++animationItem)
	{
		//アニメーションイベントのデータを取得
		nlohmann::json eventData = animationItem.value();

		//イベントタイプを取得
		std::string eventType = eventData["EventType"];

		//イベントタイプに応じて読み込むパラメーターを変更
		if (eventType == "Movement")
		{
			LoadMovementEvent(animationItem.key(), animationEvents, eventData);
		}
		else if (eventType == "Attack")
		{
			LoadAttackEvent(animationItem.key(), animationEvents, eventData);
		}
	}
}

void CombatAnimationEditor::LoadMovementEvent(const std::string& eventName, std::vector<std::unique_ptr<AnimationEvent>>& animationEvents, nlohmann::json& eventJson)
{
	//移動タイプを取得
	std::string movementType = eventJson["MovementType"];

	//移動タイプに応じて読み込むパラメーターを変更
	if (movementType == "Velocity")
	{
		LoadVelocityMovementEvent(eventName, animationEvents, eventJson);
	}
	else if (movementType == "Easing")
	{
		LoadEasingMovementEvent(eventName, animationEvents, eventJson);
	}
}

void CombatAnimationEditor::LoadVelocityMovementEvent(const std::string& eventName, std::vector<std::unique_ptr<AnimationEvent>>& animationEvents, nlohmann::json& eventJson)
{
	VelocityMovementEvent* velocityMovementEvent = new VelocityMovementEvent();
	velocityMovementEvent->eventName = eventName;
	velocityMovementEvent->eventType = EventType::kMovement;
	velocityMovementEvent->movementType = MovementType::kVelocity;
	velocityMovementEvent->startEventTime = eventJson["StartEventTime"];
	velocityMovementEvent->endEventTime = eventJson["EndEventTime"];
	velocityMovementEvent->velocity = { eventJson["Velocity"][0].get<float>(), eventJson["Velocity"][1].get<float>(), eventJson["Velocity"][2].get<float>() };
	animationEvents.push_back(std::unique_ptr<VelocityMovementEvent>(velocityMovementEvent));
}

void CombatAnimationEditor::LoadEasingMovementEvent(const std::string& eventName, std::vector<std::unique_ptr<AnimationEvent>>& animationEvents, nlohmann::json& eventJson)
{
	EasingMovementEvent* easingMovementEvent = new EasingMovementEvent();
	easingMovementEvent->eventName = eventName;
	easingMovementEvent->eventType = EventType::kMovement;
	easingMovementEvent->movementType = MovementType::kEasing;
	easingMovementEvent->startEventTime = eventJson["StartEventTime"];
	easingMovementEvent->endEventTime = eventJson["EndEventTime"];
	easingMovementEvent->targetPosition = { eventJson["TargetPosition"][0].get<float>(), eventJson["TargetPosition"][1].get<float>(), eventJson["TargetPosition"][2].get<float>() };
	animationEvents.push_back(std::unique_ptr<EasingMovementEvent>(easingMovementEvent));
}

void CombatAnimationEditor::LoadAttackEvent(const std::string& eventName, std::vector<std::unique_ptr<AnimationEvent>>& animationEvents, nlohmann::json& eventJson)
{
	AttackEvent* attackEvent = new AttackEvent();
	attackEvent->eventName = eventName;
	attackEvent->eventType = EventType::kAttack;
	attackEvent->startEventTime = eventJson["StartEventTime"];
	attackEvent->endEventTime = eventJson["EndEventTime"];
	attackEvent->attackParameters.hitCount = eventJson["HitCount"];
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
	animationEvents.push_back(std::unique_ptr<AttackEvent>(attackEvent));
}

void CombatAnimationEditor::AddAnimationEvent(std::vector<std::unique_ptr<AnimationEvent>>& animationEvents)
{
	//イベントの名前を入力
	static char newEventName[128] = { '\0' };
	ImGui::InputText("New Event Name", newEventName, sizeof(newEventName));

	//イベントタイプの選択
	static int selectedEventType = 0;
	ImGui::Combo("Event Type", &selectedEventType, EVENT_TYPES, IM_ARRAYSIZE(EVENT_TYPES));

	//アニメーションイベントを追加
	switch (static_cast<EventType>(selectedEventType))
	{
	case EventType::kMovement:
		//移動イベントの追加
		AddMovementEvent(newEventName, animationEvents);
		break;
	case EventType::kAttack:
		//攻撃イベントの追加
		AddAttackEvent(newEventName, animationEvents);
		break;
	}
}

void CombatAnimationEditor::AddMovementEvent(char* eventName, std::vector<std::unique_ptr<AnimationEvent>>& animationEvents)
{
	//移動イベントの種類を選択
	static int selectedMovementType = 0;
	ImGui::Combo("Movement Type", &selectedMovementType, MOVEMENT_TYPES, IM_ARRAYSIZE(MOVEMENT_TYPES));

	//移動イベントを追加
	switch (static_cast<MovementType>(selectedMovementType))
	{
	case MovementType::kVelocity:
		//速度移動イベントを追加
		AddVelocityMovementEvent(eventName, animationEvents);
		break;
	case MovementType::kEasing:
		//イージング移動イベントを追加
		AddEasingMovementEvent(eventName, animationEvents);
		break;
	}
}

void CombatAnimationEditor::AddVelocityMovementEvent(char* eventName, std::vector<std::unique_ptr<AnimationEvent>>& animationEvents)
{
	//速度移動イベントのパラメーターを調整
	static VelocityMovementEvent velocityMovementEvent{};
	velocityMovementEvent.eventName = eventName;
	velocityMovementEvent.eventType = EventType::kMovement;
	velocityMovementEvent.movementType = MovementType::kVelocity;
	EditVelocityMovementEvent(&velocityMovementEvent);

	//速度移動イベントを追加
	if (ImGui::Button("Add Velocity Movement Event"))
	{
		//イベントを追加
		animationEvents.push_back(std::make_unique<VelocityMovementEvent>(velocityMovementEvent)); 
		//追加後にリセット
		velocityMovementEvent = VelocityMovementEvent(); 
		//名前のバッファをリセット
		std::memset(eventName, '\0', 128);
	}
}

void CombatAnimationEditor::EditVelocityMovementEvent(VelocityMovementEvent* velocityMovementEvent)
{
	//速度移動イベントのパラメーターを調整
	ImGui::DragFloat("Start Event Time", &velocityMovementEvent->startEventTime, 0.001f);
	ImGui::DragFloat("End Event Time", &velocityMovementEvent->endEventTime, 0.001f);
	ImGui::DragFloat3("Velocity", &velocityMovementEvent->velocity.x, 0.001f);
}

void CombatAnimationEditor::AddEasingMovementEvent(char* eventName, std::vector<std::unique_ptr<AnimationEvent>>& animationEvents)
{
	//イージング移動パラメーターを調整
	static EasingMovementEvent easingMovementEvent;
	easingMovementEvent.eventName = eventName;
	easingMovementEvent.eventType = EventType::kMovement;
	easingMovementEvent.movementType = MovementType::kEasing;
	EditEasingMovementEvent(&easingMovementEvent);

	//イージング移動イベントを追加
	if (ImGui::Button("Add Easing Movement Event"))
	{
		//イベントを追加
		animationEvents.push_back(std::make_unique<EasingMovementEvent>(easingMovementEvent));
		//追加後にリセット
		easingMovementEvent = EasingMovementEvent(); 
		//名前のバッファをリセット
		std::memset(eventName, '\0', 128);
	}
}

void CombatAnimationEditor::EditEasingMovementEvent(EasingMovementEvent* easingMovementEvent)
{
	//イージング移動イベントのパラメーターを調整
	ImGui::DragFloat("Start Event Time", &easingMovementEvent->startEventTime, 0.001f);
	ImGui::DragFloat("End Event Time", &easingMovementEvent->endEventTime, 0.001f);
	ImGui::DragFloat3("Target Position", &easingMovementEvent->targetPosition.x, 0.001f);
}

void CombatAnimationEditor::AddAttackEvent(char* eventName, std::vector<std::unique_ptr<AnimationEvent>>& animationEvents)
{
	//攻撃イベントのパラメーターを調整
	static AttackEvent attackEvent;
	attackEvent.eventName = eventName;
	attackEvent.eventType = EventType::kAttack;
	EditAttackEvent(&attackEvent);

	//攻撃イベントを追加
	if (ImGui::Button("Add Attack Event"))
	{
		//イベントを追加
		animationEvents.push_back(std::make_unique<AttackEvent>(attackEvent));
		//追加後にリセット
		attackEvent = AttackEvent(); 
		//名前のバッファをリセット
		std::memset(eventName, '\0', 128);
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
		ImGui::DragInt("Hit Count", &attackEvent->attackParameters.hitCount);
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

void CombatAnimationEditor::EditAnimationEvents(std::vector<std::unique_ptr<AnimationEvent>>& animationEvents)
{
	//全てのアニメーションイベントを編集
	for (int32_t i = 0; i < animationEvents.size(); ++i)
	{
		//ツリーノードを展開
		if (ImGui::TreeNode(animationEvents[i]->eventName.c_str()))
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
			}

			//削除ボタンを追加
			if (ImGui::Button("Delete"))
			{
				//イベントを削除
				animationEvents.erase(animationEvents.begin() + i);
				//削除後、ループのインデックスを調整
				--i;
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