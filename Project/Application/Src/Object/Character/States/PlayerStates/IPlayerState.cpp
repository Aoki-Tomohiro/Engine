#include "IPlayerState.h"
#include "Application/Src/Object/Character/Player/Player.h"

Player* IPlayerState::GetPlayer() const
{
	return dynamic_cast<Player*>(GetCharacter());
}