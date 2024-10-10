#pragma once
#include "IEnemyState.h"
#include "Engine/Utilities/RandomGenerator.h"
#include "Engine/Math/MathFunction.h"

class EnemyStateRunTowardsPlayer : public IEnemyState
{
public:
	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;

private:
	Vector3 CalculateDirectionToPlayer(const Vector3& playerPosition, const Vector3& enemyPosition);

	void RotateTowardsPlayer(const Vector3& directionToPlayer);

	void HandleCloseRangeAttack();

	void PerformCloseRangeAttack(const int attack);
};

