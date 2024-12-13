/**
 * @file PlayerStateStun.cpp
 * @brief プレイヤーのスタン状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#include "PlayerStateStun.h"
#include "Application/Src/Object/Character/Player/Player.h"

void PlayerStateStun::HandleStateTransitionInternal()
{
	//通常状態に戻す
	character_->ChangeState("Idle");
}