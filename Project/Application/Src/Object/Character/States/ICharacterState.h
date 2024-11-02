#pragma once
#include "Application/Src/Object/Editors/CombatAnimationEditor/CombatAnimationEditor.h"

class ICharacterState
{
public:
	//イベント処理の基底構造体
	struct ProcessedEventData
	{
		virtual ~ProcessedEventData() = default;
		bool isActive{}; //アクティブフラグ
	};

	//速度に関するイベントの構造体
	struct ProcessedVelocityData : public ProcessedEventData
	{
		Vector3 velocity{}; //現在の速度
	};

	//イージングによる移動イベントの構造体
	struct ProcessedEasingData : public ProcessedEventData
	{
		Vector3 startPosition{};  //開始位置
		Vector3 targetPosition{}; //目標位置
	};

	//攻撃に関するイベントの構造体
	struct ProcessedAttackData : public ProcessedEventData
	{
		int32_t currentHitCount{};       //現在のヒット数
		float elapsedAttackTime{};       //攻撃判定が発生するまでの経過時間
	};

	/// <summary>
	/// デストラクタ
	/// </summary>
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
	/// 状態遷移を処理
	/// </summary>
	virtual void HandleStateTransition() = 0;

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
	/// アニメーション状態の更新
	/// </summary>
	void UpdateAnimationState();

	/// <summary>
	/// アニメーションイベントを取得
	/// </summary>
	/// <typeparam name="Type">アニメーションイベントの種類</typeparam>
	/// <returns>アニメーションイベントを取得</returns>
	template<typename Type>
	const Type* GetAnimationEvent() const;

private:
	/// <summary>
	/// アニメーションの時間を更新
	/// </summary>
	/// <param name="currentAnimationTime">現在のアニメーション時間</param>
	/// <param name="animator">アニメーター</param>
	void UpdateAnimationSpeed(const float currentAnimationTime, AnimatorComponent* animator);

	/// <summary>
	/// 全てのアニメーションイベントを処理
	/// </summary>
	/// <param name="currentAnimationTime">現在のアニメーション時間</param>
	void ProcessAnimationEvents(const float currentAnimationTime);

	/// <summary>
	/// アニメーションイベントを処理
	/// </summary>
	/// <param name="animationEvent">アニメーションイベント</param>
	/// <param name="animationTime">アニメーションの時間</param>
	void ProcessAnimationEvent(const AnimationEvent* animationEvent, const float animationTime);

	/// <summary>
	/// 移動イベントを実行
	/// </summary>
	/// <param name="movementEvent">移動イベント</param>
	/// <param name="animationTime">アニメーションの時間</param>
	void ProcessMovementEvent(const MovementEvent* movementEvent, const float animationTime);

	/// <summary>
	/// 速度移動イベントの初期化
	/// </summary>
	/// <param name="velocityMovementEvent">速度移動イベント</param>
	virtual void InitializeVelocityMovement(const VelocityMovementEvent* velocityMovementEvent) = 0;

	/// <summary>
	/// 速度移動イベントを実行
	/// </summary>
	/// <param name="velocityMovementEvent">速度移動イベント</param>
	void ProcessVelocityMovementEvent(const VelocityMovementEvent* velocityMovementEvent);

	/// <summary>
	/// イージング移動イベントの初期化
	/// </summary>
	/// <param name="easingMovementEvent">イージング移動イベント</param>
	virtual void InitializeEasingMovementEvent(const EasingMovementEvent* easingMovementEvent) = 0;

	/// <summary>
	/// イージング移動イベントを実行
	/// </summary>
	/// <param name="easingMovementEvent">イージング移動イベント</param>
	/// <param name="animationTime">アニメーションの時間</param>
	void ProcessEasingMovementEvent(const EasingMovementEvent* easingMovementEvent, const float animationTime);

	/// <summary>
	/// 攻撃イベントの初期化
	/// </summary>
	/// <param name="attackEvent">攻撃イベント</param>
	void InitializeAttackEvent(const AttackEvent* attackEvent);

	/// <summary>
	/// 攻撃イベントを実行
	/// </summary>
	/// <param name="attackEvent">攻撃イベント</param>
	void ProcessAttackEvent(const AttackEvent* attackEvent);

	/// <summary>
	/// 処理済みデータをリセット
	/// </summary>
	/// <param name="eventType">イベントの種類</param>
	void ResetProcessedData(const EventType eventType);

protected:
	//キャラクターへのポインタ
	BaseCharacter* character_ = nullptr;

	//アニメーションコントローラー
	const AnimationController* animationController_ = nullptr;

	//アニメーション速度設定のインデックス
	int32_t speedConfigIndex_ = -1;

	//速度移動イベント用の構造体
	ProcessedVelocityData processedVelocityData_{};

	//イージング移動イベント用の構造体
	ProcessedEasingData processedEasingData_{};

	//攻撃イベント用の構造体
	ProcessedAttackData processedAttackData_{};
};

template<typename Type>
inline const Type* ICharacterState::GetAnimationEvent() const
{
	//指定された型のイベントを探す
	for (const std::shared_ptr<AnimationEvent>& event : animationController_->animationEvents)
	{
		//アニメーションイベントを指定された型にキャスト
		if (const Type* castedEvent = dynamic_cast<const Type*>(event.get()))
		{
			//キャストに成功した場合そのイベントを返す
			return castedEvent;
		}
	}

	//アニメーションイベントが見つからなかった場合はnullptrを返す
	return nullptr;
}