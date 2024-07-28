#pragma once
#include "IEnemyState.h"
#include "Application/Src/Object/Warning/Warning.h"

class EnemyStateLaserAttack : public IEnemyState
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

private:
	//警告オブジェクト
	Warning* warning_ = nullptr;

	//現在の状態
	AttackState currentAttackState_ = kCharge;

	//前のフレームの状態
	AttackState preAttackState_ = kCharge;
};

