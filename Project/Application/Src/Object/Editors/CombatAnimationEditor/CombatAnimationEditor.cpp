#include "CombatAnimationEditor.h"

void CombatAnimationEditor::Initialize()
{

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
	if (ImGui::Button("Add Animation"))
	{
		AddCombatAnimation(characterAnimationData, characterAnimationData.selectedAnimationName);
	}

	//編集するアニメーションが存在しない場合は何もしない
	if (characterAnimationData.combatAnimationStates.empty()) { return; };

	//現在編集しているアニメーションの選択
	ImGui::SeparatorText("Edit Animation");
	SelectFromMap("Edit Animation", characterAnimationData.currentEditAnimationName, characterAnimationData.combatAnimationStates, false);

	//編集するアニメーションが選択されていない場合は何もしない
	if (characterAnimationData.currentEditAnimationName.empty()) { return; };

	//選択した戦闘アニメーションのコントロールを表示
	DisplayCombatAnimationControls(character->GetName(), characterAnimationData.currentEditAnimationName);

	//アニメーションの状態を取得
	CombatAnimationState& animationState = characterAnimationData.combatAnimationStates[characterAnimationData.currentEditAnimationName];

	//アニメーションフェーズを追加
	ImGui::SeparatorText("Add Animation Phase");
	AddAnimationPhase(animationState);

	//アニメーションフェーズを編集
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
	if (characterAnimationData.combatAnimationStates.find(selectedAnimationName) == characterAnimationData.combatAnimationStates.end())
	{
		characterAnimationData.combatAnimationStates[selectedAnimationName] = CombatAnimationState();
	}
}

void CombatAnimationEditor::DisplayCombatAnimationControls(const std::string& characterName, const std::string& combatAnimationName)
{
	////保存
	//if (ImGui::Button("Save"))
	//{
	//	SaveFile(characterName, combatAnimationName);
	//}

	////次のアイテムを同じ行に配置
	//ImGui::SameLine();

	////読み込み
	//if (ImGui::Button("Load"))
	//{
	//	LoadFile(characterName, combatAnimationName);
	//}
}

void CombatAnimationEditor::AddAnimationPhase(CombatAnimationState& animationState)
{
	//新しいフェーズの名前を入力
	static char newPhaseName[128] = { '\0' };
	ImGui::InputText("New Phase Name", newPhaseName, sizeof(newPhaseName));

	//新しいフェーズの追加
	if (ImGui::Button("Add Phase"))
	{
		//名前をstd::stringに格納
		std::string phaseName(newPhaseName);

		//名前が入力されていなかったら処理を飛ばす
		if (phaseName.empty()) { return; };

		//フェーズ名の重複チェック
		bool phaseExists = std::any_of(animationState.phases.begin(), animationState.phases.end(), [&phaseName](const CombatPhase& phase)
			{
				return phase.name == phaseName;
			}
		);

		//同じ名前のフェーズが存在しなければ新しいフェーズを追加
		if (!phaseExists)
		{
			CombatPhase combatPhase{};
			combatPhase.name = phaseName;
			animationState.phases.push_back(combatPhase);
			std::memset(newPhaseName, '\0', sizeof(newPhaseName));
		}
	}
}