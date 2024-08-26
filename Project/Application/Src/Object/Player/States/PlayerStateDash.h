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
	//インプット
	Input* input_ = nullptr;

	//速度
	Vector3 velocity_{};

	//アニメーションの状態
	AnimationState animationState_{};

	//アニメーションタイマー
	float animationTime_ = 0.0f;

	//パーティクル
	ParticleSystem* particleSystem_ = nullptr;
};

