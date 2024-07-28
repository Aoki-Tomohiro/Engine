#pragma once
#include "IEnemyState.h"
#include <vector>

class EnemyStateComboAttack : public IEnemyState
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

private:
	//現在の状態
	AttackState currentAttackState_ = kCharge;

	//前のフレームの状態
	AttackState preAttackState_ = kCharge;

	//現在のコンボインデックス
	int32_t comboIndex_ = 0;
};

