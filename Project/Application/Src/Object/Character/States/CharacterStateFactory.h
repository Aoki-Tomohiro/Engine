#pragma once
#include "ICharacterState.h"

/// <summary>
/// キャラクターの新しい状態を生成するクラス
/// </summary>
class CharacterStateFactory
{
public:
	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static CharacterStateFactory* GetInstance();

	/// <summary>
	/// 新しい状態を生成
	/// </summary>
	/// <param name="characterName">キャラクターの名前</param>
	/// <param name="stateName">新しい状態の名前</param>
	/// <returns>新しい状態</returns>
	ICharacterState* CreateCharacterState(const std::string& characterName, const std::string& stateName);

private:
	CharacterStateFactory() = default;
	~CharacterStateFactory() = default;
	CharacterStateFactory(const CharacterStateFactory&) = delete;
	const CharacterStateFactory& operator=(const CharacterStateFactory&) = delete;

	/// <summary>
	/// プレイヤーの新しい状態を生成
	/// </summary>
	/// <param name="stateName">新しい状態の名前</param>
	/// <returns>新しい状態</returns>
	ICharacterState* CreatePlayerState(const std::string& stateName);

	/// <summary>
	/// 敵の新しい状態を生成
	/// </summary>
	/// <param name="stateName">新しい状態の名前</param>
	/// <returns>新しい状態</returns>
	ICharacterState* CreateEnemyState(const std::string& stateName);
};

