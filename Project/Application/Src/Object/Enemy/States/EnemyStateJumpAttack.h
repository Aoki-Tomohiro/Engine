#pragma once
#include "Application/Src/Object/Enemy/States/IEnemyState.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"

class Weapon;

class EnemyStateJumpAttack : public IEnemyState
{
public:
	enum JumpAttackState
	{
		kCharge,
		kAttack,
		kRecovery,
	};

	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;

private:
	void UpdateAnimationPhase(Weapon* weapon, float currentAnimationTime);

	void UpdateVelocityForCurrentPhase();

	void ApplyParametersToWeapon();

	void HandleCurrentPhase(Weapon* weapon);

	void AttackUpdate(Weapon* weapon);

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

