#pragma once
#include "EnemyStateMove.h"

/// <summary>
/// 後進行動
/// </summary>
class EnemyStateMoveAwayFromPlayer : public EnemyStateMove
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;
};

