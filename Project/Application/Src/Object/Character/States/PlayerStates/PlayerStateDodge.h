#pragma once
#include "IPlayerState.h"
#include "Engine/Math/MathFunction.h"

/// <summary>
/// 回避状態
/// </summary>
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
	/// <param name="">衝突相手</param>
	void OnCollision(GameObject*) override {};

private:
	/// <summary>
	/// 回避時のアニメーションとアニメーションイベントの設定
	/// </summary>
	/// <param name="inputLength">入力の強さ</param>
	void ConfigureDodgeAnimationAndEvents(const float inputLength);

	/// <summary>
	/// 前方回避の場合はキャラクターを回転
	/// </summary>
	void ApplyRotationForForwardDodge();

private:
	//アニメーションの名前
	std::string animationName_{};
};

