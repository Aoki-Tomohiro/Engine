#include "PlayerStateStun.h"
#include "Application/Src/Object/Character/Player/Player.h"

void PlayerStateStun::HandleStateTransitionInternal()
{
	//通常状態に戻す
	character_->ChangeState("Idle");
}