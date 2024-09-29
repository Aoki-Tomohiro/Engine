#pragma once
#include "Application/Src/Object/Enemy/States/IEnemyState.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"
#include "Application/Src/Object/Pillar/Pillar.h"

class EnemyStateEarthSpikeAttack : public IEnemyState
{
public:
	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;

private:
	void UpdateAnimationPhase(const float currentAnimationTime);

	void CreatePillar();

	Vector3 GetDirectionToPlayer();

private:
	//アニメーションの状態
	CombatAnimationState animationState_{};

	//フェーズのインデックス
	int32_t phaseIndex_ = 0;
};

