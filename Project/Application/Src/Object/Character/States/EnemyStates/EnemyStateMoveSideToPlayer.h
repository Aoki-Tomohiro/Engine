/**
 * @file EnemyStateMoveSideToPlayer.h
 * @brief 敵の左右移動状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "EnemyStateMove.h"

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

