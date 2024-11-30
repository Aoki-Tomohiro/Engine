#pragma once
#include "Engine/Components/PostEffects/PostEffects.h"
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

	//エフェクトに関するイベントの構造体
	struct ProcessedEffectData : public ProcessedEventData
	{
		PostEffectType postEffectType{};  //ポストエフェクトの種類
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
		bool isBufferedInputActive{};       //先行入力が有効かどうか
		bool isTransitioned{};              //実際に状態遷移が発生したかどうか
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
	/// <param name="isLoop">ループフラグ</param>
	void SetAnimationControllerAndPlayAnimation(const std::string& animationName, const bool isLoop = false);

	/// <summary>
	/// アニメーション状態の更新
	/// </summary>
	void UpdateAnimationState();

	/// <summary>
	/// 状態遷移
	/// </summary>
	void HandleStateTransition();

	/// <summary>
	/// デフォルトの状態遷移処理
	/// </summary>
	virtual void HandleStateTransitionInternal() = 0;

	/// <summary>
	/// 敵と接近したかどうかを判別
	/// </summary>
	/// <returns>敵と接近したかどうか</returns>
	bool IsOpponentInProximity() const;

	/// <summary>
	/// 状態遷移が発生したかどうか
	/// </summary>
	/// <param name="actionName">遷移したアクションの名前</param>
	/// <returns>状態遷移が発生したかどうか</returns>
	bool HasStateTransitioned(const std::string& actionName = "") const;

	/// <summary>
	/// 先行入力があるか確認し、状態を遷移させる
	/// </summary>
	/// <param name="actionName">確認するアクションの名前</param>
	/// <returns>状態遷移したかどうか</returns>
	bool CheckAndTransitionBufferedAction(const std::string& actionName = "");

	//イベント初期化関連メソッド群
	virtual void InitializeVelocityMovement(const VelocityMovementEvent* velocityMovementEvent, const int32_t animationEventIndex) = 0;
	virtual void InitializeEasingMovementEvent(const EasingMovementEvent* easingMovementEvent, const int32_t animationEventIndex) = 0;
	virtual void InitializeRotationEvent(const int32_t animationEventIndex);
	virtual void InitializeAttackEvent(const AttackEvent* attackEvent, const int32_t animationEventIndex);
	virtual void InitializeEffectEvent(const EffectEvent* effectEvent, const int32_t animationEventIndex);
	virtual void InitializeCameraAnimationEvent(const CameraAnimationEvent* cameraAnimationEvent, const int32_t animationEventIndex);
	virtual void InitializeCancelEvent(const CancelEvent* cancelEvent, const int32_t animationEventIndex);
	virtual void InitializeBufferedActionEvent(const int32_t animationEventIndex);

	//イベント処理関連メソッド群
	virtual void ProcessAnimationEvents(const float currentAnimationTime);
	virtual void ProcessAnimationEvent(const AnimationEvent* animationEvent, const float animationTime, const int32_t animationEventIndex);
	virtual void ProcessMovementEvent(const MovementEvent* movementEvent, const float animationTime, const int32_t animationEventIndex);
	virtual void ProcessVelocityMovementEvent(const VelocityMovementEvent* velocityMovementEvent, const int32_t animationEventIndex);
	virtual void ProcessEasingMovementEvent(const EasingMovementEvent* easingMovementEvent, const float animationTime, const int32_t animationEventIndex);
	virtual void ProcessRotationEvent(const RotationEvent* rotationEvent, const float animationTime, const int32_t animationEventIndex);
	virtual void ProcessAttackEvent(const AttackEvent* attackEvent, const int32_t animationEventIndex);
	virtual void ProcessEffectEvent(const EffectEvent* effectEvent, const int32_t animationEventIndex);
	virtual void ApplyEffect(const EffectEvent* effectEvent, const Vector3& emitterPosition);
	virtual void ProcessCameraAnimationEvent(const CameraAnimationEvent* cameraAnimationEvent, const int32_t animationEventIndex);
	virtual void ProcessCancelEvent(const CancelEvent* cancelEvent, const int32_t animationEventIndex);
	virtual void HandleCancelAction(const CancelEvent* cancelEvent, const int32_t animationEventIndex);
	virtual void ProcessBufferedActionEvent(const BufferedActionEvent* bufferedActionEvent, const int32_t animationEventIndex);
	virtual void HandleBufferedAction(const BufferedActionEvent* bufferedActionEvent, const int32_t animationEventIndex);

private:
	/// <summary>
	/// アニメーションの時間を更新
	/// </summary>
	/// <param name="currentAnimationTime">現在のアニメーション時間</param>
	/// <param name="animator">アニメーター</param>
	void UpdateAnimationSpeed(const float currentAnimationTime, AnimatorComponent* animator);

	/// <summary>
	/// イージング係数を取得
	/// </summary>
	/// <param name="animationEvent">アニメーションイベント</param>
	/// <param name="easingType">イージングの種類</param>
	/// <param name="animationTime">アニメーションの時間</param>
	/// <returns>イージング係数</returns>
	const float GetEasingParameter(const AnimationEvent* animationEvent, const EasingType easingType, const float animationTime) const;

	/// <summary>
	/// 敵との接近を確認し、接近しすぎている場合にはキャラクターを適切に補正する関数
	/// </summary>
	/// <returns>接近補正が適用されたかどうか</returns>
	const bool ApplyProximityCorrection() const;

	//データリセット関連メソッド群
	void InitializeProcessedData();
	void ResetProcessedData(const EventType eventType, const int32_t animationEventIndex);
	void ResetMovementData(const int32_t animationEventIndex);
	void ResetRotationData(const int32_t animationEventIndex);
	void ResetAttackData(const int32_t animationEventIndex);
	void ResetEffectData(const int32_t animationEventIndex);
	void ResetCameraAnimationData(const int32_t animationEventIndex);
	void ResetCancelData(const int32_t animationEventIndex);
	void ResetBufferedActionData(const int32_t animationEventIndex);

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

	//エフェクトイベント用の構造体
	std::vector<ProcessedEffectData> processedEffectDatas_{};

	//カメラアニメーションイベントの構造体
	std::vector<ProcessedCameraAnimationData> processedCameraAnimationDatas_{};

	//キャンセルイベント用の構造体
	std::vector<ProcessedCancelData> processedCancelDatas_{};

	//先行入力用の構造体
	std::vector<ProcessedBufferedActionData> processedBufferedActionDatas_{};
};