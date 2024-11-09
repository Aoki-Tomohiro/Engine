#pragma once
#include "Application/Src/Object/Character/States/ICharacterState.h"

class Enemy;

class IEnemyState : public ICharacterState
{
public:
	virtual ~IEnemyState() override = default;

	//敵を取得
	Enemy* GetEnemy() const;

protected:
	/// <summary>
	/// 状態遷移
	/// </summary>
	virtual void HandleStateTransition() override;

	/// <summary>
	/// イージング移動イベントの初期化
	/// </summary>
	/// <param name="easingMovementEvent">イージング移動イベント</param>
	/// <param name="animationEventIndex">アニメーションイベントのインデックス</param> 
	virtual void InitializeVelocityMovement(const VelocityMovementEvent* velocityMovementEvent, const int32_t animationEventIndex) override;

	/// <summary>
	/// イージング移動イベントの初期化
	/// </summary>
	/// <param name="easingMovementEvent">イージング移動イベント</param>
	/// <param name="animationEventIndex">アニメーションイベントのインデックス</param> 
	virtual void InitializeEasingMovementEvent(const EasingMovementEvent* easingMovementEvent, const int32_t animationEventIndex) override;

private:
	/// <summary>
	/// キャンセルアクションを実行
	/// </summary>
	/// <param name="cancelEvent">キャンセルイベント</param>
	void HandleCancelAction(const CancelEvent* cancelEvent, const int32_t animationEventIndex) override;

	/// <summary>
	/// 先行入力イベントを実行
	/// </summary>
	/// <param name="bufferedActionEvent">先行入力イベント</param>
	/// <param name="animationEventIndex">アニメーションイベントのインデックス</param> 
	void HandleBufferedAction(const BufferedActionEvent* bufferedActionEvent, const int32_t animationEventIndex) override;
};