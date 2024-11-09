#pragma once
#include "IPlayerState.h"
#include "Engine/Math/MathFunction.h"

/// <summary>
/// 通常状態の処理
/// </summary>
class PlayerStateRoot : public IPlayerState
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
	/// 状態遷移
	/// </summary>
	void HandleStateTransition() override;

	/// <summary>
	/// 現在のアニメーションがIdleではない場合再生する処理
	/// </summary>
	void SetIdleAnimationIfNotPlaying();

	/// <summary>
	/// 移動処理
	/// </summary>
	/// <param name="inputValue">入力値</param>
	/// <param name="inputLength">入力の強さ</param>
	void Move(const Vector3& inputValue, const float inputLength);

	/// <summary>
	/// アニメーションの更新
	/// </summary>
	/// <param name="inputValue">入力値</param>
	/// <param name="isRunning">走っているかどうか</param>
	void UpdateAnimation(const Vector3& inputValue, bool isRunning);

	/// <summary>
	/// 歩きのアニメーションの名前を取得
	/// </summary>
	/// <param name="inputValue">入力値</param>
	const std::string DetermineWalkingAnimation(const Vector3& inputValue) const;

	/// <summary>
	/// 走りのアニメーションの名前を取得
	/// </summary>
	/// <param name="inputValue">入力値</param>
	const std::string DetermineRunningAnimation(const Vector3& inputValue) const;

	/// <summary>
	/// 回転の更新
	/// </summary>
	/// <param name="velocity">速度</param>
	void UpdateRotation(const Vector3& velocity);

private:
	//現在のアニメーションの名前
	std::string currentAnimationName_{};
};

