#include "IEnemyState.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"

Enemy* IEnemyState::GetEnemy() const
{
	return dynamic_cast<Enemy*>(GetCharacter());
}

void IEnemyState::InitializeVelocityMovement(const VelocityMovementEvent*, const int32_t)
{
}

void IEnemyState::InitializeEasingMovementEvent(const EasingMovementEvent*, const int32_t)
{
}

void IEnemyState::HandleCancelAction(const CancelEvent* cancelEvent, const int32_t animationEventIndex)
{
	//キャンセルの条件が設定されていない場合
	if (cancelEvent->cancelType == "None")
	{
		//フラグを立てる
		processedCancelDatas_[animationEventIndex].isCanceled = true;
		//デフォルトの状態遷移を行う
		HandleStateTransition();
	}
}

void IEnemyState::HandleBufferedAction(const BufferedActionEvent*, const int32_t)
{
}