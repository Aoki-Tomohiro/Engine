#pragma once
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Animator/AnimatorComponent.h"
#include "Application/Src/Object/Editors/ParticleEffectEditor/ParticleEffectEditor.h"
#include "Application/Src/Object/Editors/CombatAnimationEditor/MovementEvent.h"
#include "Application/Src/Object/Editors/CombatAnimationEditor/AttackEvent.h"
#include <Engine/Externals/nlohmann/json.hpp>
#include <fstream>
#include <map>

class BaseCharacter;

/// <summary>
/// 戦闘のアニメーションを管理するクラス
/// </summary>
class CombatAnimationEditor
{
public:
	//キャラクターのアニメーションデータ
	struct CharacterAnimationData
	{
		std::map<std::string, std::vector<std::unique_ptr<AnimationEvent>>> combatAnimations; // キャラクターのアニメーションイベント
		std::string selectedAnimationName;                                                    // 選択しているアニメーションの名前
		std::string currentEditAnimationName;                                                 // 現在編集中のアニメーションの名前
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

	/// <summary>
	/// 全てのアニメーションイベントを取得
	/// </summary>
	/// <param name="characterName">キャラクターの名前
	/// <param name="animationName">アニメーションの名前</param>
	/// <returns>アニメーションイベント</returns>
	const std::vector<std::unique_ptr<AnimationEvent>> GetAnimationEvents(const std::string& characterName, const std::string& animationName) const;

	//パーティクルエフェクトエディターを設定
	void SetParticleEffectEditor(const ParticleEffectEditor* particleEffectEditor) { particleEffectEditor_ = particleEffectEditor; };

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
	/// ファイルに保存
	/// </summary>
	/// <param name="characterName">キャラクターの名前</param>
	/// <param name="combatAnimationName">戦闘アニメーションの名前</param>
	void SaveFile(const std::string& characterName, const std::string& combatAnimationName);

	/// <summary>
	/// 移動イベントを保存
	/// </summary>
	/// <param name="movementEvent">移動イベント</param>
	/// <param name="eventJson">イベントのjsonObject</param>
	void SaveMovementEvent(MovementEvent* movementEvent, nlohmann::json& eventJson);

	/// <summary>
	/// 速度移動イベントを保存
	/// </summary>
	/// <param name="velocityMovementEvent">速度イベント</param>
	/// <param name="eventJson">イベントのjsonObject</param>
	void SaveVelocityMovementEvent(VelocityMovementEvent* velocityMovementEvent, nlohmann::json& eventJson);

	/// <summary>
	/// イージング移動イベントを保存
	/// </summary>
	/// <param name="easingMovementEvent">イージング移動イベント</param>
	/// <param name="eventJson">イベントのjsonobject</param>
	void SaveEasingMovementEvent(EasingMovementEvent* easingMovementEvent, nlohmann::json& eventJson);

	/// <summary>
	/// 攻撃ベントを保存
	/// </summary>
	/// <param name="attackEvent">攻撃イベント</param>
	/// <param name="eventJson">イベントのjsonobject</param>
	void SaveAttackEvent(AttackEvent* attackEvent, nlohmann::json& eventJson);

	/// <summary>
	/// 全てのファイルを保存
	/// </summary>
	void LoadFiles();

	/// <summary>
	/// ファイルを保存
	/// </summary>
	/// <param name="characterName">キャラクターの名前</param>
	/// <param name="combatAnimationName">戦闘アニメーションの名前</param>
	void LoadFile(const std::string& characterName, const std::string& combatAnimationName);

	/// <summary>
	/// 移動イベントを読み込む
	/// </summary>
	/// <param name="eventName">イベントの名前</param>
	/// <param name="animationEvents">アニメーションイベントの配列</param>
	/// <param name="eventJson">アニメーションイベントのjsonobject</param>
	void LoadMovementEvent(const std::string& eventName, std::vector<std::unique_ptr<AnimationEvent>>& animationEvents, nlohmann::json& eventJson);

	/// <summary>
	/// 速度移動イベントを読み込む
	/// </summary>
	/// <param name="eventName">イベントの名前</param>
	/// <param name="animationEvents">アニメーションイベントの配列</param>
	/// <param name="eventJson">アニメーションイベントのjsonobject</param>
	void LoadVelocityMovementEvent(const std::string& eventName, std::vector<std::unique_ptr<AnimationEvent>>& animationEvents, nlohmann::json& eventJson);

	/// <summary>
	/// イージング移動イベントを読み込む
	/// </summary>
	/// <param name="eventName">イベントの名前</param>
	/// <param name="animationEvents">アニメーションイベントの配列</param>
	/// <param name="eventJson">アニメーションイベントのjsonobject</param>
	void LoadEasingMovementEvent(const std::string& eventName, std::vector<std::unique_ptr<AnimationEvent>>& animationEvents, nlohmann::json& eventJson);

	/// <summary>
	/// 攻撃イベントを読み込む
	/// </summary>
	/// <param name="eventName">イベントの名前</param>
	/// <param name="animationEvents">アニメーションイベントの配列</param>
	/// <param name="eventJson">アニメーションイベントのjsonobject</param>
	void LoadAttackEvent(const std::string& eventName, std::vector<std::unique_ptr<AnimationEvent>>& animationEvents, nlohmann::json& eventJson);

	/// <summary>
	/// アニメーションイベントを追加
	/// </summary>
	/// <param name="animationEvents">アニメーションイベントの配列</param>
	void AddAnimationEvent(std::vector<std::unique_ptr<AnimationEvent>>& animationEvents);

	/// <summary>
	/// 移動イベントを追加
	/// </summary>
	/// <param name="eventName">イベントの名前</param>
	/// <param name="animationEvents">アニメーションイベントの配列</param>
	void AddMovementEvent(char* eventName, std::vector<std::unique_ptr<AnimationEvent>>& animationEvents);

	/// <summary>
	/// 速度移動イベントを追加
	/// </summary>
	/// <param name="eventName">イベントの名前</param>
	/// <param name="animationEvents">アニメーションイベントの配列</param>
	void AddVelocityMovementEvent(char* eventName, std::vector<std::unique_ptr<AnimationEvent>>& animationEvents);

	/// <summary>
	/// 速度による移動イベントを編集
	/// </summary>
	/// <param name="velocityMovementEvent">速度移動イベント</param>
	void EditVelocityMovementEvent(VelocityMovementEvent* velocityMovementEvent);

	/// <summary>
	/// イージング移動イベントを追加
	/// </summary>
	/// <param name="eventName">イベントの名前</param>
	/// <param name="animationEvents">アニメーションイベントの配列</param>
	void AddEasingMovementEvent(char* eventName, std::vector<std::unique_ptr<AnimationEvent>>& animationEvents);

	/// <summary>
	/// イージングによる移動イベントを編集
	/// </summary>
	/// <param name="easingMovementEvent">イージング移動イベント</param>
	void EditEasingMovementEvent(EasingMovementEvent* easingMovementEvent);

	/// <summary>
	/// 攻撃イベントを追加
	/// </summary>
	/// <param name="eventName">イベントの名前</param>
	/// <param name="animationEvents">アニメーションイベントの配列</param>
	void AddAttackEvent(char* eventName, std::vector<std::unique_ptr<AnimationEvent>>& animationEvents);

	/// <summary>
	/// 攻撃イベントを編集
	/// </summary>
	/// <param name="attackEvent">攻撃イベント</param>
	void EditAttackEvent(AttackEvent* attackEvent);

	/// <summary>
	/// アニメーションイベントを編集
	/// </summary>
	/// <param name="animationEvents">アニメーションイベントの配列</param>
	void EditAnimationEvents(std::vector<std::unique_ptr<AnimationEvent>>& animationEvents);

	/// <summary>
	/// 移動イベントを編集
	/// </summary>
	/// <param name="velocityMovementEvent">移動イベント</param>
	void EditMovementEvent(MovementEvent* movementEvent);

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
	//アニメーションイベント
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