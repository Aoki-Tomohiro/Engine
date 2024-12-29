/**
 * @file PlayerStateRoot.h
 * @brief プレイヤーの通常状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "AbstractPlayerState.h"
#include "Engine/Math/MathFunction.h"

class PlayerStateRoot : public AbstractPlayerState
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
	/// デフォルトの状態遷移処理
	/// </summary>
	void HandleStateTransitionInternal() override;

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
	/// アニメーション名を決定
	/// </summary>
	/// <param name="inputValue">入力値</param>
	/// <param name="isRunning">走り状態かどうか</param>
	/// <returns>アニメーション名</returns>
	std::string GetAnimationNameBasedOnLockon(const Vector3& inputValue, bool isRunning) const;

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
	/// アニメーション名の末尾の数字を取り出し逆方向のアニメーションかどうかを判定する関数
	/// </summary>
	/// <param name="currentAnimationName">現在のアニメーション</param>
	/// <param name="animationName">次のアニメーション</param>
	/// <returns>逆方向のアニメーションかどうか</returns>
	bool IsOppositeAnimation(const std::string& currentAnimationName, const std::string& animationName);

	/// <summary>
	/// 回転の更新
	/// </summary>
	/// <param name="velocity">速度</param>
	void UpdateRotation(const Vector3& velocity);

private:
	//現在のアニメーションの名前
	std::string currentAnimationName_{};

	//前回の入力値
	Vector3 previousInputValue_;

	//現在の移動速度
	float currentMoveSpeed_ = 0.0f;
};

