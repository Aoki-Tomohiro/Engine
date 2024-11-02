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
	/// <param name="velocityMovementEvent">速度移動イベント</param>
	virtual void InitializeVelocityMovement(const VelocityMovementEvent* velocityMovementEvent) override;

	/// <summary>
	/// イージング移動イベントの初期化
	/// </summary>
	/// <param name="easingMovementEvent">イージング移動イベント</param>
	virtual void InitializeEasingMovementEvent(const EasingMovementEvent* easingMovementEvent) override;

protected:
	//インプット
	Input* input_ = nullptr;
};