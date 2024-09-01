#pragma once
#include "Application/Src/Object/Enemy/States/IEnemyState.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"

class Weapon;

class EnemyStateComboAttack : public IEnemyState
{
public:
	enum ComboAttackState
	{
		kCharge1,
		kWarning1,
		kAttack1,
		kCharge2,
		kWarning2,
		kAttack2,
		kCharge3,
		kWarning3,
		kAttack3,
		kRecovery,
	};

	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;

private:
	void UpdateAnimationPhase(Weapon* weapon, float currentAnimationTime);

	void RotateTowardsPlayer();

	void UpdateVelocityForCurrentPhase();

	void ApplyParametersToWeapon();

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

