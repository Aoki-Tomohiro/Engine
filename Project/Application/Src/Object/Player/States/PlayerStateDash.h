#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/AnimationStateManager/AnimationStateManager.h"

class PlayerStateDash : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

private:
	//ダッシュの状態
	enum DashState
	{
		kCharge,
		kDash,
		kRecovery,
	};

	void HandleLockon();

	void HandleNonLockon();

	void InitializeParticleSystem();

	void UpdateAnimationPhase();

	void DashUpdate();

	void RecoveryUpdate();

	void SpawnDashParticles();

	void UpdateEmitterPosition();

	void ResetDashFlags();

private:
	//インプット
	Input* input_ = nullptr;

	//速度
	Vector3 velocity_{};

	//アニメーションの状態
	AnimationState animationState_{};

	//前のアニメーションのフェーズ
	uint32_t prePhaseIndex_ = 0;

	//現在のアニメーションのフェーズ
	uint32_t currentPhaseIndex_ = 0;

	//アニメーションタイマー
	float animationTime_ = 0.0f;

	//パーティクル
	ParticleSystem* particleSystem_ = nullptr;
};

