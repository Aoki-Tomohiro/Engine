#pragma once
#include "IPlayerState.h"
#include "Engine/Math/MathFunction.h"

/// <summary>
/// ダッシュ状態
/// </summary>
class PlayerStateDash : public IPlayerState
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
	/// 速度移動イベントを実行
	/// </summary>
	/// <param name="velocityMovementEvent">速度移動イベント</param>
	/// <param name="animationEventIndex">アニメーションイベントのインデックス</param> 
	void InitializeVelocityMovement(const VelocityMovementEvent* velocityMovementEvent, const int32_t animationEventIndex) override;

	/// <summary>
	/// RadialBlurの更新
	/// </summary>
	void UpdateRadialBlur();

	/// <summary>
	/// ダッシュアニメーション終了の確認と遷移
	/// </summary>
	void CheckTransitionToDashEndAnimation();

	/// <summary>
	/// ダッシュ終了処理
	/// </summary>
	void FinalizeDash();

private:
	//現在のアニメーションの名前
	std::string currentAnimation_ = "DashStart";

	//RadialBlurの調整項目
	float blurWidthDefault_ = 0.04f;
};

