/**
 * @file EnemyStateIdle.cpp
 * @brief 敵の通常状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#include "EnemyStateIdle.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"

void EnemyStateIdle::Initialize()
{
	//基底クラスの初期化
	EnemyStateMove::Initialize();

	//待機時間をランダムに設定
	InitializeRandomWaitTimeBeforeMovement(GetEnemy()->GetRootParameters().minWaitTimeBeforeMovement, GetEnemy()->GetRootParameters().maxWaitTimeBeforeMovement);

	//アニメーションを再生
	SetAnimationControllerAndPlayAnimation("Idle", true);
}