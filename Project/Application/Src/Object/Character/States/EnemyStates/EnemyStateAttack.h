#pragma once
#include "IEnemyState.h"

/// <summary>
/// 攻撃状態
/// </summary>
class EnemyStateAttack : public IEnemyState
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="animationName">アニメーションの名前</param>
	EnemyStateAttack(const std::string& animationName) : animationName_(animationName) {};

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
	/// 攻撃をランダムで設定
	/// </summary>
	void SetRandomAttack();

protected:
	//アニメーションの名前
	std::string animationName_{};

	//ジャスト回避タイマー
	float justDodgeTimer_ = 0.0f;
};

