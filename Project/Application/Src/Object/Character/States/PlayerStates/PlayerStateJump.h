/**
 * @file PlayerStateJump.h
 * @brief プレイヤーのジャンプ状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "AbstractPlayerState.h"
#include "Engine/Math/MathFunction.h"

class PlayerStateJump : public AbstractPlayerState
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
	/// ジャンプ時のアニメーションとアニメーションイベントの設定
	/// </summary>
	/// <param name="inputLength">入力の強さ</param>
	void ConfigureJumpAnimationAndEvents(const float inputLength);

	/// <summary>
	/// 重力を適用
	/// </summary>
	void ApplyGravity();

	/// <summary>
	/// ロックオン中かどうかを判別して敵の方向に向ける処理
	/// </summary>
	void LookAtLockonTarget();

	/// <summary>
	/// 地面に着地しているかを確認
	/// </summary>
	void CheckLandingAndTransitionState();
};

