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
	float timeToActivateHitbox_ = 1.8f;

	//攻撃終了時間
	float timeToDeactivateHitbox_ = timeToActivateHitbox_ + 1.35f;

	//攻撃用タイマー
	float attackTimer_ = 0.0f;
};

