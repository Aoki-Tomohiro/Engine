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
	//チャージが終了したかどうか
	bool isChargeFinished_ = false;

	//発射可能かどうか
	bool isFireEnabled_ = false;

	//チャージタイマー
	float chargeTimer_ = 0.0f;

	//発射タイマー
	float fireTimer_ = 0.0f;

	//発射回数
	uint32_t fireCount_ = 0;

	//硬直フラグ
	bool isRecovery_ = false;

	//硬直タイマー
	float recoveryTimer_ = 0.0f;
};

