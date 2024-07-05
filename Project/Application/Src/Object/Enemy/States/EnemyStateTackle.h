#pragma once
#include "IEnemyState.h"

class EnemyStateTackle : public IEnemyState
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera);

    const bool GetIsAttack() const { return isAttack_; };

private:
	float chargeDuration_ = 1.8f;

	float chargeTimer_ = 0.0f;
};

