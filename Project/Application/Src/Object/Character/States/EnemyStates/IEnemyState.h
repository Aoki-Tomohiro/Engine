#pragma once
#include "Application/Src/Object/Character/States/ICharacterState.h"

class Enemy;

/// <summary>
/// 敵の状態
/// </summary>
class IEnemyState : public ICharacterState
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IEnemyState() override = default;

	//敵を取得
	Enemy* GetEnemy() const;

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
	/// プレイヤーへの方向ベクトルを計算する関数
	/// </summary>
	/// <returns>プレイヤーへの方向ベクトル</returns>
	Vector3 CalculateDirectionToPlayer() const;
};