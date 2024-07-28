#pragma once
#include "IEnemyState.h"

class EnemyStateDead : public IEnemyState
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

private:
	bool isDissolveEnable_ = false;

	float deathTimer_ = 0.0f;
};

