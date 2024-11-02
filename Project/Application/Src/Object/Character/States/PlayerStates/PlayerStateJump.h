#pragma once
#include "IPlayerState.h"
#include "Engine/Math/MathFunction.h"

/// <summary>
/// ジャンプ状態
/// </summary>
class PlayerStateJump : public IPlayerState
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
	/// 状態遷移を処理
	/// </summary>
	void HandleStateTransition() override;

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
	/// 速度移動イベントの初期化
	/// </summary>
	/// <param name="velocityMovementEvent">速度移動イベント</param>
	void InitializeVelocityMovement(const VelocityMovementEvent* velocityMovementEvent) override;
};

