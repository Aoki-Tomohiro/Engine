#pragma once
#include "EnemyStateMove.h"

/// <summary>
/// 待機状態
/// </summary>
class EnemyStateIdle : public EnemyStateMove
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;
};

