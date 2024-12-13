/**
 * @file EnemyStateMoveTowardPlayer.h
 * @brief 敵の前進状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "EnemyStateMove.h"

class EnemyStateMoveTowardPlayer : public EnemyStateMove
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;
};

