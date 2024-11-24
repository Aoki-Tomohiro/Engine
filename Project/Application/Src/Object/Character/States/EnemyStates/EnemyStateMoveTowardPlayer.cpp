#include "EnemyStateMoveTowardPlayer.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"

void EnemyStateMoveTowardPlayer::Initialize()
{
	//基底クラスの初期化
	EnemyStateMove::Initialize();

	//待機時間をランダムに設定
	InitializeRandomWaitTimeBeforeMovement(GetEnemy()->GetRootParameters().minWaitTimeBeforeMovement, GetEnemy()->GetRootParameters().maxWaitTimeBeforeMovement);

	//アニメーションを再生
	SetAnimationControllerAndPlayAnimation(character_->GetIsGameFinished() ? "Idle" : "Walk1", true);
}