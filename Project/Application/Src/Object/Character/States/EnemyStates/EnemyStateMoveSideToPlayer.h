#pragma once
#include "EnemyStateMove.h"

/// <summary>
/// 左右移動状態
/// </summary>
class EnemyStateMoveSideToPlayer : public EnemyStateMove
{
public:
	//移動方向
	enum class WalkDirection
	{
		kLeft,  //左歩き
		kRight  //右歩き
	};

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;
};

