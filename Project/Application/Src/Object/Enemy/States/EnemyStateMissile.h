#pragma once
#include "IEnemyState.h"
#include "Application/Src/Object/Missile/Missile.h"

class EnemyStateMissile : public IEnemyState
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

private:
	void CreateMissile(const Missile::MissileParameters& missileParameters, const Vector3& velocity);

private:
	//現在の攻撃の状態
	AttackState attackState_ = kCharge;

	//発射タイマー
	float fireTimer_ = 0.0f;

	//現在の発射数
	int32_t fireCount_ = 0;
};

