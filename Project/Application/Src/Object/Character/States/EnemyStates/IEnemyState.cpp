#include "IEnemyState.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"

Enemy* IEnemyState::GetEnemy() const
{
	return dynamic_cast<Enemy*>(GetCharacter());
}

void IEnemyState::HandleStateTransition()
{
}

void IEnemyState::InitializeVelocityMovement(const VelocityMovementEvent*, const int32_t)
{
}

void IEnemyState::InitializeEasingMovementEvent(const EasingMovementEvent*, const int32_t)
{
}

void IEnemyState::HandleCancelAction(const CancelEvent*, const int32_t)
{
}

void IEnemyState::HandleBufferedAction(const BufferedActionEvent*, const int32_t)
{

}