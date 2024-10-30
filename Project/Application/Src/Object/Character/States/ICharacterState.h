#pragma once
#include "Application/Src/Object/Editors/CombatAnimationEditor/CombatAnimationEditor.h"

class ICharacterState
{
public:
	virtual ~ICharacterState() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 衝突処理
	/// </summary>
	/// <param name="other">衝突相手</param>
	virtual void OnCollision(GameObject* other) = 0;

	//キャラクターを取得・設定
	BaseCharacter* GetCharacter() const { return character_; };
	void SetCharacter(BaseCharacter* character) { character_ = character; };

protected:
	/// <summary>
	/// 全てのアニメーションイベントを実行
	/// </summary>
	void ProcessAnimationEvents();

	/// <summary>
	/// アニメーションイベントが有効かどうかを判定
	/// </summary>
	/// <param name="eventName">アニメーションイベントの名前</param>
	/// <returns>アニメーションイベントが有効かどうか</returns>
	const bool IsAnimationEventActive(const std::string& eventName) const;

private:
	/// <summary>
	/// アニメーションイベントを実行
	/// </summary>
	/// <param name="animationEvent">アニメーションイベント</param>
	/// <param name="animationTime">アニメーションの時間</param>
	void ProcessAnimationEvent(AnimationEvent* animationEvent, const float animationTime);

	/// <summary>
	/// 移動イベントを実行
	/// </summary>
	/// <param name="movementEvent">移動イベント</param>
	/// <param name="animationTime">アニメーションの時間</param>
	void ProcessMovementEvent(MovementEvent* movementEvent, const float animationTime);

	/// <summary>
	/// 速度移動イベントを実行
	/// </summary>
	/// <param name="velocityMovementEvent">速度移動イベント</param>
	void ProcessVelocityMovementEvent(VelocityMovementEvent* velocityMovementEvent);

	/// <summary>
	/// イージング移動イベントを実行
	/// </summary>
	/// <param name="easingMovementEvent">イージング移動イベント</param>
	/// <param name="animationTime">アニメーションの時間</param>
	void ProcessEasingMovementEvent(EasingMovementEvent* easingMovementEvent, const float animationTime);

	/// <summary>
	/// 攻撃イベントを実行
	/// </summary>
	/// <param name="attackEvent">攻撃イベント</param>
	void ProcessAttackEvent(AttackEvent* attackEvent);

private:
	//キャラクターへのポインタ
	BaseCharacter* character_ = nullptr;

	//アニメーションイベント
	std::vector<std::unique_ptr<AnimationEvent>> animationEvents_{};
};