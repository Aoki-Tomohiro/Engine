#pragma once
#include "IEnemyState.h"

class EnemyStateIdle : public IEnemyState
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

private:
	float moveSpeed_ = 0.2f;

	float minAttackDuration_ = 2.0f;

	float maxAttackDuration_ = 4.0f;

	float attackDuration_ = 1.0f;

	float attackTimer_ = 0.0f;
};

