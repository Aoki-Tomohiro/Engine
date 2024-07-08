#pragma once
#include "IEnemyState.h"

class EnemyStateIdle : public IEnemyState
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
	/// 描画
	/// </summary>
	/// <param name="camera"></param>
	void Draw(const Camera& camera) override;

private:
	//移動速度
	float moveSpeed_ = 0.2f;

	//攻撃までの最小時間
	float minAttackDuration_ = 2.0f;

	//攻撃までの最大時間
	float maxAttackDuration_ = 4.0f;

	//攻撃までの時間
	float attackDuration_ = 1.0f;

	//攻撃用のタイマー
	float attackTimer_ = 0.0f;
};

