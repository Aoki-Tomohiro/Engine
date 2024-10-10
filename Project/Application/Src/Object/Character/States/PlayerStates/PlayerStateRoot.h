#pragma once
#include "Application/Src/Object/Character/States/PlayerStates/IPlayerState.h"

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
	/// <param name="gameObject">衝突相手</param>
	void OnCollision(GameObject* gameObject) override;

private:
	/// <summary>
	/// 現在のアニメーションがIdleではない場合再生する処理
	/// </summary>
	void SetIdleAnimationIfNotPlaying();

	/// <summary>
	/// 移動処理
	/// </summary>
	/// <param name="inputValue">スティックの入力値</param>
	/// <param name="inputLength">スティックの入力の強さ</param>
	void Move(const Vector3& inputValue, const float inputLength);

	/// <summary>
	/// アニメーションの更新
	/// </summary>
	/// <param name="inputValue">入力値</param>
	/// <param name="isRunning">走っている状態かどうか</param>
	void UpdateAnimation(const Vector3& inputValue, bool isRunning);

	/// <summary>
	/// スティック入力に基づいて歩きアニメーション名を決定する処理
	/// </summary>
	/// <param name="inputValue">スティックの入力値</param>
	/// <returns>アニメーションの名前</returns>
	const std::string DetermineWalkingAnimation(const Vector3& inputValue) const;

	/// <summary>
	/// スティック入力に基づいて走りアニメーション名を決定する処理
	/// </summary>
	/// <param name="inputValue">スティックの入力値</param>
	/// <returns>アニメーションの名前</returns>
	const std::string DetermineRunningAnimation(const Vector3& inputValue) const;

	/// <summary>
	/// 回転処理
	/// </summary>
	/// <param name="velocity">速度</param>
	void UpdateRotation(const Vector3& velocity);

	/// <summary>
	/// 状態遷移処理
	/// </summary>
	void ProcessStateTransition();

private:
	//インプット
	Input* input_ = nullptr;

	//現在のアニメーションの名前
	std::string currentAnimationName_{};
};

