#include "IEnemyState.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"

Enemy* IEnemyState::GetEnemy() const
{
	return dynamic_cast<Enemy*>(GetCharacter());
}