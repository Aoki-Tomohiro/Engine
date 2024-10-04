#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"
#include "Application/Src/Object/MagicProjectile/MagicProjectile.h"

class PlayerStateMagicAttack : public IPlayerState
{
public:
	enum MagicAttackState
	{
		kCharge,
		kFire,
		kRecovery,
	};

	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;

private:
	void SetMagicAttackAnimation(const std::string& animationName);

	void UpdateAnimationPhase(float currentAnimationTime);

	void ExecuteMagicAttack();

	void ResetToFirePhase();

	void UpdateRotation();

	void CreateMagicProjectile();

	const Vector3 GetMagicProjectileVelocity();

	void SetMagicProjectileTransform(MagicProjectile* magicProjectile);

	bool CheckRecoveryCancel();

private:
	//インプット
	Input* input_ = nullptr;

	//オーディオ
	Audio* audio_ = nullptr;

	//アニメーションの状態
	CombatAnimationState animationState_{};

	//フェーズのインデックス
	int32_t phaseIndex_ = 0;

	//オーディオハンドル
	uint32_t fireAudioHandle_ = 0;
};

