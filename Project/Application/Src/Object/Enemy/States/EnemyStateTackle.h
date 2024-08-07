#pragma once
#include "IEnemyState.h"
#include "Application/Src/Object/Warning/Warning.h"

class EnemyStateTackle : public IEnemyState
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

private:
	//警告オブジェクト
	Warning* warning_ = nullptr;

	//現在の状態
	AttackState currentTackleState_ = kCharge;

	//前のフレームの状態
	AttackState preTackleState_ = kCharge;

	//始点座標
	Vector3 startPosition_{};

	//目標座標
	Vector3 targetPosition_{};
};

