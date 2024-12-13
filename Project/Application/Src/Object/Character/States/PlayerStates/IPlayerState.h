/**
 * @file IPlayerState.h
 * @brief プレイヤーの状態の基底クラスを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Application/Src/Object/Character/States/ICharacterState.h"
#include "Engine/Components/Input/Input.h"

class Player;

class IPlayerState : public ICharacterState
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IPlayerState() override = default;

	//プレイヤーを取得
	Player* GetPlayer() const;

protected:
	/// <summary>
	/// 速度移動イベントの初期化
	/// </summary>
	/// <param name="easingMovementEvent">速度移動イベント</param>
	/// <param name="animationEventIndex">アニメーションイベントのインデックス</param> 
	void InitializeVelocityMovement(const VelocityMovementEvent* velocityMovementEvent, const int32_t animationEventIndex) override;

	/// <summary>
	/// イージング移動イベントの初期化
	/// </summary>
	/// <param name="easingMovementEvent">イージング移動イベント</param>
	/// <param name="animationEventIndex">アニメーションイベントのインデックス</param> 
	void InitializeEasingMovementEvent(const EasingMovementEvent* easingMovementEvent, const int32_t animationEventIndex) override;

private:
	/// <summary>
	/// デフォルトの状態遷移処理
	/// </summary>
	virtual void HandleStateTransitionInternal() override;

	/// <summary>
	/// プレイヤーと敵の座標差分を計算する関数
	/// </summary>
	/// <returns></returns>
	Vector3 CalculateDirectionToEnemy() const;

	/// <summary>
	/// スティック入力に基づいて移動ベクトルを計算する共通関数
	/// </summary>
	/// <param name="vector">ベクトル</param>
	/// <param name="walkThreshold">歩きの閾値</param>
	/// <returns>移動ベクトル</returns>
	Vector3 ProcessStickInputMovement(const Vector3& vector, const float walkThreshold) const;

protected:
	//インプット
	Input* input_ = nullptr;

	//Y成分の差が許容範囲内かどうかを判定するための閾値
	const float maxAllowableYDifference = 4.0f;
};