#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"
#include "Application/Src/Object/Magic/Magic.h"

class PlayerStateChargeMagicAttack : public IPlayerState
{
public:
	enum ChargeMagicAttackState
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

	void UpdateVelocityForPhase();

	void CreateMagicProjectile();

	const Vector3 GetMagicProjectileVelocity();

	void SetMagicProjectileTransform(Magic* magic);

	void CreateChargeMagicParticles();

private:
	//インプット
	Input* input_ = nullptr;

	//オーディオ
	Audio* audio_ = nullptr;

	//アニメーションの状態
	CombatAnimationState animationState_{};

	//フェーズのインデックス
	int32_t phaseIndex_ = 0;

	//速度
	Vector3 velocity_{};

	//加速度
	Vector3 acceleration_{};

	//オーディオハンドル
	uint32_t chargeMagicAudioHandle_ = 0;
};

