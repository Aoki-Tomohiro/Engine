/**
 * @file EnemyStateMoveSideToPlayer.cpp
 * @brief 敵の左右移動状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#include "EnemyStateMoveSideToPlayer.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"

void EnemyStateMoveSideToPlayer::Initialize()
{
	//基底クラスの初期化
	EnemyStateMove::Initialize();

	//待機時間をランダムに設定
	InitializeRandomWaitTimeBeforeMovement(GetEnemy()->GetRootParameters().minWaitTimeBeforeMovement, GetEnemy()->GetRootParameters().maxWaitTimeBeforeMovement);

	//ランダムな歩き方向を選択
	WalkDirection direction = static_cast<WalkDirection>(RandomGenerator::GetRandomInt(0, 1));

	//アニメーション名を決定
	std::string animationName = (direction == WalkDirection::kRight) ? "Walk3" : "Walk4";

	//アニメーションを再生
	SetAnimationControllerAndPlayAnimation(character_->GetIsGameFinished() ? "Idle" : animationName, true);
}