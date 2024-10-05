#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"

class Weapon;

class PlayerStateLaunchAttack : public IPlayerState
{
public:
	enum LaunchAttackState
	{
		kCharge,
		kFirstAttack,
		kNormalAttack,
		kRecovery,
	};

	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;

private:
	void CheckForAttackStateTransition();

	void UpdateAnimationPhase(Weapon* weapon, float currentAnimationTime);

	void HandleCurrentPhase(Weapon* weapon);

	void AttackUpdate(Weapon* weapon);

	void RecoveryUpdate(Weapon* weapon);

	void CreateLaunchParticles(const Vector3& emitterPosition);

	void CreateAccelerationField(const Vector3& accelerationPosition);

	Vector3 GetInputValue();

private:
	//インプット
	Input* input_ = nullptr;

	//アニメーションの状態
	CombatAnimationState animationState_{};

	//フェーズのインデックス
	int32_t phaseIndex_ = 0;

	//ヒットカウント
	int32_t hitCount_ = 0;

	//ヒットタイマー
	float hitTimer_ = 0.0f;

	//攻撃状態への遷移するかどうか
	bool isAttackStateTransitionRequested_ = false;

	//パーティクルのオフセット値
	Vector3 launchParticleOffset_{ 0.0f, 0.0f, 3.0f };

	//パーティクルの速度
	Vector3 launchParticleVelocity_{ 0.0f, 0.06f, 0.2f };

	//パーティクルの加速度
	Vector3 launchParticleAcceleration_{ 0.0f, 2.8f, -0.8f };
};

