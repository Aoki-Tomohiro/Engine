#pragma once
#include "IEnemyState.h"

/// <summary>
/// 通常状態
/// </summary>
class EnemyStateMove : public IEnemyState
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~EnemyStateMove() override = default;

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
	/// <param name="other">衝突相手</param>
	void OnCollision(GameObject* other) override;

protected:
	/// <summary>
	/// 待機時間をランダムに設定する関数
	/// </summary>
	/// <param name="minWaitTime">最小待ち時間</param>
	/// <param name="maxWaitTime">最大待ち時間</param>
	void InitializeRandomWaitTimeBeforeMovement(const float minWaitTime, const float maxWaitTime);

private:
	/// <summary>
	/// 移動ベクトルの回転処理
	/// </summary>
	void RotateVelocity();

	/// <summary>
	/// 攻撃状態への遷移を試行
	/// </summary>
	/// <returns>攻撃状態に遷移したかどうか</returns>
	bool TryTransitionToAttackState();

	/// <summary>
	/// 攻撃行動を決める
	/// </summary>
	/// <returns>攻撃行動に遷移したかどうか</returns>
	bool DecideAttackAction();

	/// <summary>
	/// 回避状態への遷移を試行
	/// </summary>
	/// <returns>回避状態に遷移したかどうか</returns>
	bool TryTransitionToDodgeState();

	/// <summary>
	/// 待機タイマーを更新
	/// </summary>
	void UpdateMovementWaitTimer();

protected:
	//接近前の待機時間
	float currentWaitTimeBeforeMovement_ = 0.0f;

	//接近前の待機タイマー
	float waitTimeBeforeMovementTimer_ = 0.0f;
};

