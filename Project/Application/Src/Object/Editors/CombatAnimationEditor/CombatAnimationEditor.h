#pragma once
#include "MovementParameters.h"
#include "AttackParameters.h"
#include "Engine/Base/ImGuiManager.h"
#include "Application/Src/Object/Character/BaseCharacter.h"
#include "Application/Src/Object/Editors/ParticleEffectEditor/ParticleEffectEditor.h"
#include <Engine/Externals/nlohmann/json.hpp>
#include <fstream>
#include <map>

//フェーズ情報を持つ構造体
struct CombatPhase
{
	std::vector<std::unique_ptr<ActionParameters>> actionParameters{}; //全てのパラメーター
	std::string name{};                                                //名前
	float duration;                                                    //持続時間
};

//アニメーション状態とその関連フェーズを管理する構造体
struct CombatAnimationState
{
	std::vector<CombatPhase> phases{}; //全てのフェーズ
};

/// <summary>
/// 戦闘アニメーションを編集するクラス
/// </summary>
class CombatAnimationEditor
{
public:
	//キャラクターのアニメーションデータ
	struct CharacterAnimationData
	{
		std::map<std::string, CombatAnimationState> combatAnimationStates{}; //戦闘アニメーション
		std::string selectedAnimationName{};                                 // 選択しているアニメーションの名前
		std::string currentEditAnimationName{};                              // 現在編集中のアニメーションの名前
	};

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 編集するキャラクターを追加
	/// </summary>
	/// <param name="character">キャラクター</param>
	void AddEditableCharacter(BaseCharacter* character);

private:
	//保存先ファイルパス
	const std::string kDirectoryPath = "Application/Resources/Config/CombatAnimations/";

	/// <summary>
	/// キャラクターのアニメーションを編集
	/// </summary>
	/// <param name="character">キャラクター</param>
	void EditCharacterAnimations(BaseCharacter* character);

	/// <summary>
	/// 選択中のアニメーションの時間を編集
	/// </summary>
	/// <param name="animator">アニメーター</param>
	/// <param name="selectedAnimationName">選択中のアニメーションの名前</param>
	void EditAnimationTime(AnimatorComponent* animator, const std::string& selectedAnimationName);

	/// <summary>
	/// 戦闘のアニメーションを追加
	/// </summary>
	/// <param name="characterAnimationData">キャラクターのアニメーションデータ</param>
	/// <param name="selectedAnimationName">選択中のアニメーションの名前</param>
	void AddCombatAnimation(CharacterAnimationData& characterAnimationData, const std::string& selectedAnimationName);

	/// <summary>
	/// 戦闘アニメーションのコントロールを表示
	/// </summary>
	/// <param name="characterName">キャラクターの名前</param>
	/// <param name="combatAnimationName">戦闘アニメーションの名前</param>
	void DisplayCombatAnimationControls(const std::string& characterName, const std::string& combatAnimationName);

	/// <summary>
	/// アニメーションフェーズを追加
	/// </summary>
	/// <param name="animationState">アニメーションステート</param>
	void AddAnimationPhase(CombatAnimationState& animationState);

	/// <summary>
	/// コンテナの中から要素を選択するためのComboBoxを作成する関数
	/// </summary>
	/// <typeparam name="Type">コンテナに格納されている値の型</typeparam>
	/// <param name="label">ラベル名</param>
	/// <param name="selectedName">選択された要素の名前</param>
	/// <param name="items">表示する名前とその設定が格納されたコンテナ</param>
	/// <param name="showLabel">ラベルを表示するかどうか</param>
	template<typename Type>
	void SelectFromMap(const char* label, std::string& selectedName, const std::map<std::string, Type>& items, const bool showLabel);

private:
	//アニメーション
	std::map<std::string, CharacterAnimationData> characterAnimations_{};

	//編集するキャラクター
	std::map<std::string, BaseCharacter*> editableCharacters_{};

	//パーティクルエフェクトエディター
	const ParticleEffectEditor* particleEffectEditor_ = nullptr;
};


template<typename Type>
inline void CombatAnimationEditor::SelectFromMap(const char* label, std::string& selectedName, const std::map<std::string, Type>& items, const bool showLabel)
{
	//ラベルを表示
	if (showLabel)
	{
		ImGui::Text(label);
	}

	//コンボボックスの作成
	if (ImGui::BeginCombo(label, selectedName.c_str()))
	{
		//全てのアイテムのキーをリスト表示
		for (const auto& [name, setting] : items)
		{
			//選択されたかどうかを判別
			const bool isSelected = (selectedName == name);

			//選択されていた場合、名前を更新
			if (ImGui::Selectable(name.c_str(), isSelected))
			{
				selectedName = name;
			}

			//選択されていた場合はデフォルトフォーカスを設定
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		//コンボボックスの終了
		ImGui::EndCombo();
	}
}