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
	//溜め時間
	float chargeDuration_ = 2.07f;

	//攻撃予告時間
	float warningDuration_ = 2.26f;

	//攻撃時間
	float attackDuration_ = 2.9f;
};

