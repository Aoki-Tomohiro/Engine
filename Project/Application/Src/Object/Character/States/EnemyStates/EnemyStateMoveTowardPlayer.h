#pragma once
#include "EnemyStateMove.h"

/// <summary>
/// 前進行動
/// </summary>
class EnemyStateMoveTowardPlayer : public EnemyStateMove
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;
};

