#pragma once
#include "IEnemyState.h"

/// <summary>
/// 接近行動
/// </summary>
class EnemyStateApproach : public IEnemyState
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
	/// <param name="other">衝突相手</param>
	void OnCollision(GameObject* other) override;

private:
	/// <summary>
	/// 接近行動をランダムに決定
	/// </summary>
	void SetRandomApproachAction();

	/// <summary>
	/// 速度ベクトルを回転させる
	/// </summary>
	void RotateVelocity();

private:
	//アニメーションの名前
	std::string animationName_{};
};
