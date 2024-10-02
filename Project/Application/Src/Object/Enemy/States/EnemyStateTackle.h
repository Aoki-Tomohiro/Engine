#pragma once
#include "Application/Src/Object/Enemy/States/IEnemyState.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"

class Weapon;

class EnemyStateTackle : public IEnemyState
{
public:
	enum TackleAttackState
	{
		kCharge,
		kWait,
		kAttack,
		kRecovery,
	};

	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;

private:
	void UpdateAnimationPhase(Weapon* weapon, float currentAnimationTime);

	void UpdateVelocityForCurrentPhase();

	void HandleCurrentPhase(Weapon* weapon);

	void ChargeUpdate();

	void AttackUpdate(Weapon* weapon);

	Vector3 GetDirectionToPlayer();

private:
	//アニメーションの状態
	CombatAnimationState animationState_{};

	//フェーズのインデックス
	int32_t phaseIndex_ = 0;

	//速度
	Vector3 velocity_{};

	//移動が終了しているかどうか
	bool isMovementFinished = false;

	//攻撃がヒットしたかどうか
	bool isHit_ = false;
};

