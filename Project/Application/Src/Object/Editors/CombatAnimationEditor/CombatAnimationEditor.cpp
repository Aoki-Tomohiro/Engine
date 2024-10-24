#include "CombatAnimationEditor.h"

void CombatAnimationEditor::Initialize()
{

}

void CombatAnimationEditor::Update()
{
	//タブバーを開始
	if (ImGui::BeginTabBar("Characters"))
	{
		//設定されている全てのキャラクターのアイテムを作成
		for (const auto& editableCharacter : editableCharacters_)
		{
			if (ImGui::BeginTabItem(editableCharacter.first.c_str()))
			{
				//キャラクターにアニメーションを編集
				EditCharacter(editableCharacter.second);
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

	//同じキャラクターが存在しなければキャラクターのコンテナに追加
	if (editableCharacters_.find(characterName) == editableCharacters_.end())
	{
		editableCharacters_[characterName] = character;
		characterAnimations_[characterName] = CharacterAnimationData();
	}
}

void CombatAnimationEditor::EditCharacter(BaseCharacter* character)
{
	//キャラクターのアニメーションデータを取得
	CharacterAnimationData characterAnimationData = characterAnimations_[character->GetName()];

	//現在編集しているアニメーションの名前
	static std::string currentEditAnimationName;

	//編集するアニメーションを選択
	SelectCombatAnimation(character->GetAnimator(), currentEditAnimationName);

	//編集するアニメーションを追加
	if (ImGui::Button("Add Animation Event"))
	{
		AddCombatAnimation(characterAnimationData, currentEditAnimationName);
	}

	//編集するアニメーションが存在しなければ処理を飛ばす
	if (characterAnimationData.animationEvents.empty()) { return; };
}

void CombatAnimationEditor::SelectCombatAnimation(AnimatorComponent* animator, std::string& animationName)
{
	//ComboBoxの作成
	if (ImGui::BeginCombo("Animations", animationName.c_str()))
	{
		//全てのアイテムのキーをリスト表示
		for (const auto& [name, animation] : animator->GetAnimations())
		{
			//選択されたかどうかを判別
			const bool isSelected = (animationName == name);

			//選択されていた場合、名前を更新
			if (ImGui::Selectable(name.c_str(), isSelected))
			{
				animationName = name;
			}

			//選択されていた場合はデフォルトフォーカスを設定
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		//ComboBoxの終了
		ImGui::EndCombo();
	}
}

void CombatAnimationEditor::AddCombatAnimation(CharacterAnimationData& characterAnimationData, const std::string& currentEditAnimationName)
{
	//同じアニメーションイベントのコンテナが存在しなければ追加する
	if (characterAnimationData.animationEvents.find(currentEditAnimationName) == characterAnimationData.animationEvents.end())
	{
		characterAnimationData.animationEvents[currentEditAnimationName] = std::vector<AnimationEvent>();
	}
}