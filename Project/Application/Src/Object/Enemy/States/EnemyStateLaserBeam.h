#pragma once
#include "Application/Src/Object/Enemy/States/IEnemyState.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"
#include "Application/Src/Object/Laser/Laser.h"

class EnemyStateLaserBeam : public IEnemyState
{
public:
	enum LaserBeamState
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
	void UpdateAnimationPhase(const float currentAnimationTime);

	void CreateLaser();

	void HandleCurrentPhase();

	void ChargeUpdate();

	void AttackUpdate();

	Vector3 GetDirectionToPlayer();

private:
	//アニメーションの状態
	CombatAnimationState animationState_{};

	//フェーズのインデックス
	int32_t phaseIndex_ = 0;

	//レーザー
	Laser* laser_ = nullptr;
};

