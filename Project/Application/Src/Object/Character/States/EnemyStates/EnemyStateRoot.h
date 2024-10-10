#pragma once
#include "IEnemyState.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/RandomGenerator.h"
#include <optional>

class EnemyStateRoot : public IEnemyState
{
public:
	//移動状態
	enum class MovementBehavior
	{
		kIdle,
		kTowardsPlayer,
		kRetreat,
		kSideways,
	};

	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;

private:
	void UpdateActionTimer();

	void PerformActionBasedOnDistance();

	void HandleCloseRangeAttack(float distanceToPlayer, float stopDistance);

	void HandleRangedAttack();

	void PerformApproachAction(const int action);

	void PerformCloseRangeAttack(const int attack);

	void PerformRangedAttack(const int attack);

	void ResetBlendDurationAfterAnimation(float defaultBlendDuration);

	void RotateTowardsPlayer();

	void InitializeMovementBehavior();

	void UpdateMovementBehavior();

	void InitializeIdle();

	void UpdateIdle();

	void InitializeTowardsPlayer();

	void UpdateTowardsPlayer();

	void InitializeRetreat();

	void UpdateRetreat();

	void InitializeSideways();

	void UpdateSideways();

	void ResetWaitTimeBeforeMovement();

	void UpdateWaitTimeAndRequestMovement(const MovementBehavior& behavior);

private:
	//振る舞い
	MovementBehavior behavior_ = MovementBehavior::kIdle;

	//次の振る舞い
	MovementBehavior nextBehavior_ = MovementBehavior::kIdle;

	//次のふるまいのリクエスト
	std::optional<MovementBehavior> behaviorRequest_ = std::nullopt;

	//現在のアニメーション
	std::string currentAnimation_ = "Idle";

	//速度
	Vector3 velocity_{};

	//横移動の方向
	float horizontalMovementDirection_ = 1.0f;

	//現在の次の行動の間隔
	float currentActionInterval_ = 0.0f;

	//アクションタイマー
	float actionTimer_ = 0.0f;

	//接近前の待機時間
	float currentWaitTimeBeforeMovement_ = 0.0f;

	//接近前の待機タイマー
	float waitTimeBeforeMovementTimer_ = 0.0f;
};

