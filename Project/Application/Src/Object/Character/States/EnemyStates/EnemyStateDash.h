#pragma once
#include "IEnemyState.h"
#include "Engine/Utilities/RandomGenerator.h"
#include "Engine/Math/MathFunction.h"

class EnemyStateDash : public IEnemyState
{
public:
	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;

private:
	const Vector3 CalculateEndPosition() const;

	void FinishDash();

	Vector3 CalculateDirectionToPlayer() const;

	const bool ShouldStartCloseRangeAttack() const;

	void TransitionToRandomState();

	float GetEasingParameter() const;

	void UpdateDashMovement();

	void HandleCloseRangeAttack();

	void PerformCloseRangeAttack(const int attack);

	void RotateTowardsPlayer();

	const float GetCurrentAnimationTime() const;

	const float GetAnimationDuration() const;

private:
	//開始座標
	Vector3 startPosition_{};

	//目標座標
	Vector3 endPosition_{};

	//ダッシュが終了したかどうか
	bool isDashFinished_ = false;
};

