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

	//回転に関するイベントの構造体
	struct ProcessedRotationData : public ProcessedEventData
	{
		float preEasedRotation{}; //累積回転量
	};

	//攻撃に関するイベントの構造体
	struct ProcessedAttackData : public ProcessedEventData
	{
		int32_t currentHitCount{}; //現在のヒット数
		float elapsedAttackTime{}; //攻撃判定が発生するまでの経過時間
	};

	//カメラアニメーションに関するイベントの構造体
	struct ProcessedCameraAnimationData : public ProcessedEventData
	{
		bool isAnimationStarted{}; //アニメーションが開始されたかどうか
	};

	//キャンセルに関数イベントの構造体
	struct ProcessedCancelData : public ProcessedEventData
	{
		std::string cancelActionName{}; //キャンセルするアクションの名前
		bool isCanceled{};              //キャンセルしたかどうか
	};

	//先行入力に関数イベントの構造体
	struct ProcessedBufferedActionData : public ProcessedEventData
	{
		std::string bufferedActionName{};   //先行入力のアクションの名前
		bool isBufferedInputActive{};       //先行入力で状態を遷移したかどうか
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
	/// 衝突処理
	/// </summary>
	/// <param name="other">衝突相手</param>
	virtual void OnCollision(GameObject* other) = 0;

	//キャラクターを取得・設定
	BaseCharacter* GetCharacter() const { return character_; };
	void SetCharacter(BaseCharacter* character) { character_ = character; };

protected:
	/// <summary>
	/// アニメーションコントローラーを取得し、アニメーションを再生
	/// </summary>
	/// <param name="animationName">アニメーションの名前</param>
	void SetAnimationControllerAndPlayAnimation(const std::string& animationName);

	/// <summary>
	/// アニメーション状態の更新
	/// </summary>
	void UpdateAnimationState();

	/// <summary>
	/// 先行入力があるか確認し、状態を遷移させる
	/// </summary>
	/// <returns>状態遷移したかどうか</returns>
	const bool CheckAndTransitionBufferedAction();

	/// <summary>
	/// 状態遷移
	/// </summary>
	virtual void HandleStateTransition() = 0;

private:
	/// <summary>
	/// 各パラメーターを初期化
	/// </summary>
	void InitializeProcessedData();

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
	/// <param name="animationEventIndex">アニメーションイベントのインデックス</param> 
	void ProcessAnimationEvent(const AnimationEvent* animationEvent, const float animationTime, const int32_t animationEventIndex);

	/// <summary>
	/// 移動イベントを実行
	/// </summary>
	/// <param name="movementEvent">移動イベント</param>
	/// <param name="animationTime">アニメーションの時間</param>
	/// <param name="animationEventIndex">アニメーションイベントのインデックス</param> 
	void ProcessMovementEvent(const MovementEvent* movementEvent, const float animationTime, const int32_t animationEventIndex);

	/// <summary>
	/// 速度移動イベントの初期化
	/// </summary>
	/// <param name="velocityMovementEvent">速度移動イベント</param>
	/// <param name="animationEventIndex">アニメーションイベントのインデックス</param> 
	virtual void InitializeVelocityMovement(const VelocityMovementEvent* velocityMovementEvent, const int32_t animationEventIndex) = 0;

	/// <summary>
	/// 速度移動イベントを実行
	/// </summary>
	/// <param name="velocityMovementEvent">速度移動イベント</param>
	/// <param name="animationEventIndex">アニメーションイベントのインデックス</param> 
	void ProcessVelocityMovementEvent(const VelocityMovementEvent* velocityMovementEvent, const int32_t animationEventIndex);

	/// <summary>
	/// イージング移動イベントの初期化
	/// </summary>
	/// <param name="easingMovementEvent">イージング移動イベント</param>
	/// <param name="animationEventIndex">アニメーションイベントのインデックス</param> 
	virtual void InitializeEasingMovementEvent(const EasingMovementEvent* easingMovementEvent, const int32_t animationEventIndex) = 0;

	/// <summary>
	/// イージング移動イベントを実行
	/// </summary>
	/// <param name="easingMovementEvent">イージング移動イベント</param>
	/// <param name="animationTime">アニメーションの時間</param>
	/// <param name="animationEventIndex">アニメーションイベントのインデックス</param> 
	void ProcessEasingMovementEvent(const EasingMovementEvent* easingMovementEvent, const float animationTime, const int32_t animationEventIndex);

	/// <summary>
	/// 回転イベントを実行
	/// </summary>
	/// <param name="rotationEvent">回転イベント</param>
	/// <param name="animationTime">アニメーションの時間</param>
	/// <param name="animationEventIndex">アニメーションイベントのインデックス</param>
	void ProcessRotationEvent(const RotationEvent* rotationEvent, const float animationTime, const int32_t animationEventIndex);

	/// <summary>
	/// 攻撃イベントの処理
	/// </summary>
	/// <param name="attackEvent">攻撃イベント</param>
	/// <param name="animationEventIndex">アニメーションイベントのインデックス</param> 
	void ProcessAttackEvent(const AttackEvent* attackEvent, const int32_t animationEventIndex);

	/// <summary>
	/// カメラアニメーションイベントの処理
	/// </summary>
	/// <param name="cameraAnimationEvent">カメラアニメーションイベント</param>
	/// <param name="animationEventIndex">アニメーションイベントのインデックス</param>
	void ProcessCameraAnimationEvent(const CameraAnimationEvent* cameraAnimationEvent, const int32_t animationEventIndex);

	/// <summary>
	/// キャンセルイベントの処理
	/// </summary>
	/// <param name="cancelEvent">キャンセルイベント</param>
	void ProcessCancelEvent(const CancelEvent* cancelEvent, const int32_t animationEventIndex);

	/// <summary>
	/// キャンセルアクションを実行
	/// </summary>
	/// <param name="cancelEvent">キャンセルイベント</param>
	virtual void HandleCancelAction(const CancelEvent* cancelEvent, const int32_t animationEventIndex) = 0;

	/// <summary>
	/// 先行入力イベントの処理
	/// </summary>
	/// <param name="bufferedActionEvent">先行入力イベント</param>
	/// <param name="animationEventIndex">アニメーションイベントのインデックス</param> 
	void ProcessBufferedActionEvent(const BufferedActionEvent* bufferedActionEvent, const int32_t animationEventIndex);

	/// <summary>
	/// 先行入力イベントを実行
	/// </summary>
	/// <param name="bufferedActionEvent">先行入力イベント</param>
	/// <param name="animationEventIndex">アニメーションイベントのインデックス</param> 
	virtual void HandleBufferedAction(const BufferedActionEvent* bufferedActionEvent, const int32_t animationEventIndex) = 0;

	/// <summary>
	/// 相手が一定以内にいるか確認
	/// </summary>
	/// <param name="proximityDistance">距離</param>
	/// <returns>相手が一定以内にいるかどうか</returns>
	const bool IsOpponentInProximity(const float proximityDistance) const;

	/// <summary>
	/// 処理済みデータをリセット
	/// </summary>
	/// <param name="eventType">イベントの種類</param>
	/// <param name="animationEventIndex">アニメーションイベントのインデックス</param>
	void ResetProcessedData(const EventType eventType, const int32_t animationEventIndex);

protected:
	//移動制限
	const float kProximityDistance = 4.0f;

	//キャラクターへのポインタ
	BaseCharacter* character_ = nullptr;

	//アニメーションコントローラー
	const AnimationController* animationController_ = nullptr;

	//アニメーション速度設定のインデックス
	int32_t speedConfigIndex_ = -1;

	//速度移動イベント用の構造体
	std::vector<ProcessedVelocityData> processedVelocityDatas_{};

	//イージング移動イベント用の構造体
	std::vector<ProcessedEasingData> processedEasingDatas_{};

	//回転移動イベント用の構造体
	std::vector<ProcessedRotationData> processedRotationDatas_{};

	//攻撃イベント用の構造体
	std::vector<ProcessedAttackData> processedAttackDatas_{};

	//カメラアニメーションイベントの構造体
	std::vector<ProcessedCameraAnimationData> processedCameraAnimationDatas_{};

	//キャンセルイベント用の構造体
	std::vector<ProcessedCancelData> processedCancelDatas_{};

	//先行入力用の構造体
	std::vector<ProcessedBufferedActionData> processedBufferedActionDatas_{};
};