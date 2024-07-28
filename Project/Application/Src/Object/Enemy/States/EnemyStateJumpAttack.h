#pragma once
#include "IEnemyState.h"

class EnemyStateJumpAttack : public IEnemyState
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

private:
	//Colliderの中心点
	Vector3 currentCenter_{};

	//腰のJointからColliderの中心点までのオフセット
	Vector3 offset_{};

	//現在の状態
	AttackState currentTackleState_ = kCharge;

	//前のフレームの状態
	AttackState preTackleState_ = kCharge;

	//開始座標
	Vector3 startPosition_{};

	//目標座標
	Vector3 targetPosition_{};

	//追尾するかどうか
	bool isTracking_ = false;
};

