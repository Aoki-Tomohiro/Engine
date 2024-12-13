/**
 * @file EnemyStateIdle.h
 * @brief 敵の通常状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "EnemyStateMove.h"

class EnemyStateIdle : public EnemyStateMove
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;
};

