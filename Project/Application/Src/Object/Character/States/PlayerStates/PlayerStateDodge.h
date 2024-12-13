/**
 * @file PlayerStateDodge.h
 * @brief プレイヤーの回避状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "IPlayerState.h"
#include "Engine/Math/MathFunction.h"

class PlayerStateDodge : public IPlayerState
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
	/// ジャスト回避の演出を更新
	/// </summary>
	void UpdateJustDodgeEffect();

	/// <summary>
	/// 回避時のアニメーションとアニメーションイベントの設定
	/// </summary>
	/// <param name="inputLength">入力の強さ</param>
	void ConfigureDodgeAnimationAndEvents(const float inputLength);

private:
	//ジャスト回避演出用のタイマー
	float justDodgeTimer_ = 0.0f;

	//ジャスト回避アニメーション開始時間
	float justDodgeStartTime_ = 1.4f;
};

