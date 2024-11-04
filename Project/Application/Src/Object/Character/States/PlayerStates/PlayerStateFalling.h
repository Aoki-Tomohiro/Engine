#pragma once
#include "IPlayerState.h"
#include "Engine/Math/MathFunction.h"

/// <summary>
/// 落下状態
/// </summary>
class PlayerStateFalling : public IPlayerState
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

	/// <summary>
	/// 空中にいる際のアニメーションの管理
	/// </summary>
	void HandleAirborneAnimation();

private:
	//着地フラグ
	bool isLanding_ = false;
};

