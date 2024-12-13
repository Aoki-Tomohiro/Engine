/**
 * @file CharacterStateFactory.cpp
 * @brief キャラクターの新しい状態を生成するファクトリー
 * @author 青木智滉
 * @date
 */

#include "CharacterStateFactory.h"
#include "Application/Src/Object/Character/States/CharacterStateDeath.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateRoot.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateJump.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateFalling.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateDodge.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateDash.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateAttack.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateMagicAttack.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateAbility.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateStun.h"
#include "Application/Src/Object/Character/States/EnemyStates/EnemyStateIdle.h"
#include "Application/Src/Object/Character/States/EnemyStates/EnemyStateMoveTowardPlayer.h"
#include "Application/Src/Object/Character/States/EnemyStates/EnemyStateMoveAwayFromPlayer.h"
#include "Application/Src/Object/Character/States/EnemyStates/EnemyStateMoveSideToPlayer.h"
#include "Application/Src/Object/Character/States/EnemyStates/EnemyStateDodge.h"
#include "Application/Src/Object/Character/States/EnemyStates/EnemyStateApproach.h"
#include "Application/Src/Object/Character/States/EnemyStates/EnemyStateAttack.h"
#include "Application/Src/Object/Character/States/EnemyStates/EnemyStateStun.h"

CharacterStateFactory* CharacterStateFactory::GetInstance()
{
	static CharacterStateFactory instance;
	return &instance;
}

ICharacterState* CharacterStateFactory::CreateCharacterState(const std::string& characterName, const std::string& stateName)
{
	//死亡状態に遷移
	if (stateName == "Death")
	{
		return new CharacterStateDeath();
	}
	//キャラクターの名前がプレイヤーだった場合
	else if (characterName == "Player")
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
	else if (stateName == "Jump" || stateName == "Stomp")
	{
		return new PlayerStateJump();
	}
	//落下状態に遷移
	else if (stateName == "Falling" || stateName == "FallingAttack")
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
	//攻撃状態に遷移
	else if (stateName == "Attack")
	{
		return new PlayerStateAttack();
	}
	//魔法攻撃状態に遷移
	else if (stateName == "Magic" || stateName == "ChargeMagic")
	{
		return new PlayerStateMagicAttack();
	}
	//アビリティ状態に遷移
	else if (stateName == "Ability1" || stateName == "Ability2")
	{
		size_t pos = stateName.find_first_of("0123456789");
		return new PlayerStateAbility(std::stoi(stateName.substr(pos)));
	}
	//スタン状態に遷移
	else if (stateName == "Stun")
	{
		return new PlayerStateStun();
	}

	//一致する名前の状態がなければnullptrを返す
	return nullptr;
}

ICharacterState* CharacterStateFactory::CreateEnemyState(const std::string& stateName)
{
	//通常状態
	if (stateName == "Idle" || stateName == "Move")
	{
		return new EnemyStateIdle();
	}
	//前進状態
	else if (stateName == "MoveTowardPlayer")
	{
		return new EnemyStateMoveTowardPlayer();
	}
	//後進状態
	else if (stateName == "MoveAwayFromPlayer")
	{
		return new EnemyStateMoveAwayFromPlayer();
	}
	//左右移動状態
	else if (stateName == "MoveSideToPlayer")
	{
		return new EnemyStateMoveSideToPlayer();
	}
	//回避状態
	else if (stateName == "Dodge")
	{
		return new EnemyStateDodge();
	}
	//接近状態
	else if (stateName == "Approach")
	{
		return new EnemyStateApproach();
	}
	//攻撃状態
	else if (stateName == "Attack" || stateName == "VerticalSlash" || stateName == "SpinAttack" || stateName == "ComboSlash1" || stateName == "ComboSlash2")
	{
		return new EnemyStateAttack(stateName);
	}
	//スタン状態
	else if (stateName == "Stun")
	{
		return new EnemyStateStun();
	}

	//一致する名前の状態がなければnullptrを返す
	return nullptr;
}