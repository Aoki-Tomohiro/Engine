#include "CharacterStateFactory.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateRoot.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateJump.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateFalling.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateDodge.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateDash.h"

CharacterStateFactory* CharacterStateFactory::GetInstance()
{
	static CharacterStateFactory instance;
	return &instance;
}

ICharacterState* CharacterStateFactory::CreateCharacterState(const std::string& characterName, const std::string& stateName)
{
	//キャラクターの名前がプレイヤーだった場合
	if (characterName == "Player")
	{
		//新しいプレイヤーの状態を生成
		return CreatePlayerState(stateName);
	}
	//キャラクターの名前が敵だった場合
	else if (characterName == "Enemy")
	{
		//新しい敵の状態を生成
		return CreateEnemyState(stateName);
	}

	return nullptr;
}

ICharacterState* CharacterStateFactory::CreatePlayerState(const std::string& stateName)
{
	//通常状態に遷移
	if (stateName == "Idle" || stateName == "Move")
	{
		return new PlayerStateRoot();
	}
	//ジャンプ状態に遷移
	else if (stateName == "Jump")
	{
		return new PlayerStateJump();
	}
	//落下状態に遷移
	else if (stateName == "Falling")
	{
		return new PlayerStateFalling();
	}
	//回避状態に遷移
	else if (stateName == "Dodge")
	{
		return new PlayerStateDodge();
	}
	//ダッシュ状態に遷移
	else if (stateName == "Dash")
	{
		return new PlayerStateDash();
	}

	//一致する名前の状態がなければnullptrを返す
	return nullptr;
}

ICharacterState* CharacterStateFactory::CreateEnemyState(const std::string& stateName)
{
	//通常状態
	if (stateName == "Idle")
	{
		return new PlayerStateRoot();
	}

	//一致する名前の状態がなければnullptrを返す
	return nullptr;
}