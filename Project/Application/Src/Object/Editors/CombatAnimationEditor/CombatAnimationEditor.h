#pragma once
#include "MovementEvent.h"
#include "AttackEvent.h"
#include "Engine/Base/ImGuiManager.h"
#include "Application/Src/Object/Character/BaseCharacter.h"
#include <unordered_map>



/// <summary>
/// 戦闘のアニメーションを管理するクラス
/// </summary>
class CombatAnimationEditor
{
public:
	//キャラクターのアニメーションデータ
	struct CharacterAnimationData
	{
		std::unordered_map<std::string, std::vector<AnimationEvent>> animationEvents; // キャラクターのアニメーションイベント
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
	/// <summary>
	/// キャラクターを編集
	/// </summary>
	/// <param name="character">キャラクター</param>
	void EditCharacter(BaseCharacter* character);

	/// <summary>
	/// 編集するアニメーションを選択
	/// </summary>
	/// <param name="animator">アニメーター</param>
	/// <param name="animationName">アニメーションの名前</param>
	void SelectCombatAnimation(AnimatorComponent* animator, std::string& animationName);

	/// <summary>
	/// アニメーションを追加
	/// </summary>
	/// <param name="characterAnimationData">キャラクターのアニメーションデータ</param>
	/// <param name="currentEditAnimationName">編集中のアニメーションの名前</param>
	void AddCombatAnimation(CharacterAnimationData& characterAnimationData, const std::string& currentEditAnimationName);

private:
	//アニメーションイベント
	std::unordered_map<std::string, CharacterAnimationData> characterAnimations_{};

	//編集するキャラクター
	std::unordered_map<std::string, BaseCharacter*> editableCharacters_{};
};

