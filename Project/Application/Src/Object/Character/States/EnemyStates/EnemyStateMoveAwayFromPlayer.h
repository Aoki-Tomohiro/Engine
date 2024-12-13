/**
 * @file EnemyStateMoveAwayFromPlayer.h
 * @brief 敵の後退状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "EnemyStateMove.h"

class EnemyStateMoveAwayFromPlayer : public EnemyStateMove
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;
};

