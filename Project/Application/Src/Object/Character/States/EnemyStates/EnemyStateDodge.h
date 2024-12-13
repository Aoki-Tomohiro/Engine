/**
 * @file EnemyStateDodge.h
 * @brief 敵の回避状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "IEnemyState.h"

class EnemyStateDodge : public IEnemyState
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 衝突処理
	/// </summary>
	/// <param name="">衝突相手</param>
	void OnCollision(GameObject*) override {};
};

