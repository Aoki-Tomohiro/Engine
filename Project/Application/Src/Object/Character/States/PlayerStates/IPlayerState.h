#pragma once
#include "Application/Src/Object/Character/States/ICharacterState.h"
#include "Engine/Components/Input/Input.h"

class Player;

/// <summary>
/// プレイヤー
/// </summary>
class IPlayerState : public ICharacterState
{
public:
	//アクションの種類
	enum class ActionType
	{
		kMove,
		kDodge,
		kDash,
		kAttack,
		kLaunchAttack,
		kSpinAttack,
	};

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IPlayerState() override = default;

	//プレイヤーを取得
	Player* GetPlayer() const;

protected:
	/// <summary>
	/// 状態遷移
	/// </summary>
	/// <param name="isAnimationCorrectionActive">アニメーションの補正をするかどうか</param>
	virtual void HandleStateTransition(const bool isAnimationCorrectionActive) override;

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

protected:
	//インプット
	Input* input_ = nullptr;
};