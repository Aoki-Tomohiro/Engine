#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"

class PlayerStateDash : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;

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

	void UpdateAnimationPhase();

	void DashUpdate();

	void RecoveryUpdate();

	void UpdateEmitterPosition();

	void HandlePhaseChange();

	void ResetDashFlags();

	void HandleAnimationFinish();

private:
	//インプット
	Input* input_ = nullptr;

	//オーディオ
	Audio* audio_ = nullptr;

	//速度
	Vector3 velocity_{};

	//アニメーションの状態
	CombatAnimationState animationState_{};

	//前のアニメーションのフェーズ
	uint32_t prePhaseIndex_ = 0;

	//現在のアニメーションのフェーズ
	uint32_t currentPhaseIndex_ = 0;

	//アニメーションタイマー
	float animationTime_ = 0.0f;

	//ダッシュのオーディオハンドル
	uint32_t dashAudioHandle_ = 0;

	//煙のパーティクルのオフセット値
	Vector3 dashParticleOffset_ = { 0.0f,0.0f,-2.0f };
};

