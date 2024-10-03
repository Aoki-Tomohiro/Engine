#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"

class Weapon;

class PlayerStateFallingAttack : public IPlayerState
{
public:
	enum FallingAttackState
	{
		kRush,
		kCharge,
		kAttack,
		kSlamAttack,
		kRecovery,
	};

	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;

private:
	void EndFallingAttack();

	void CorrectPlayerPosition();

	void UpdateAnimationPhase(Weapon* weapon, float currentAnimationTime);

	void HandlePhaseTransition(Weapon* weapon);

	void HandleCurrentPhase(Weapon* weapon);

	void AttackUpdate(Weapon* weapon);

	void HandleWeaponHit(Weapon* weapon);

	void CreateSlamAttackParticles();

	const float GetCurrentAnimationTime() const;

private:
	//インプット
	Input* input_ = nullptr;

	//アニメーションの状態
	CombatAnimationState animationState_{};

	//フェーズのインデックス
	int32_t phaseIndex_ = 0;

	//イージング係数
	float easingParameter_ = 0.0f;

	//開始座標
	Vector3 startPosition_{};

	//終了座標
	Vector3 endPosition_{};

	//ヒットタイマー
	float hitTimer_ = 0.0f;

	//ヒットカウント
	int32_t hitCount_ = 0;
};

