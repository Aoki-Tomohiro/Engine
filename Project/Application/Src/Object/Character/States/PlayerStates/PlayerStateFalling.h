/**
 * @file PlayerStateFalling.h
 * @brief プレイヤーの落下状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "AbstractPlayerState.h"
#include "Engine/Math/MathFunction.h"

class PlayerStateFalling : public AbstractPlayerState
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
	/// 重力を適用
	/// </summary>
	void ApplyGravity();

	/// <summary>
	/// 垂直位置の確認と調整
	/// </summary>
	void CheckAndHandleLanding();

	/// <summary>
	/// 着地処理
	/// </summary>
	/// <param name="landingPosition">着地座標</param>
	void ProcessLanding(const Vector3& landingPosition);

private:
	//着地フラグ
	bool isCurrentlyLanding_ = false;

	//現在のアニメーションを停止する時間
	float animationPauseThreshold_ = 0.0f;

	//落下アニメーションを停止する時間
	float fallingAnimationPauseTime_ = 0.3f;

	//落下攻撃アニメーションの開始時間
	float fallingAttackStartTime_ = 1.4f;

	//落下攻撃アニメーションを停止する時間
	float fallingAttackPauseTime_ = 1.78f;
};

