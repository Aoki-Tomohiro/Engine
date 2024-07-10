#pragma once
#include "IPlayerState.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GlobalVariables.h"

class PlayerStateDash : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void OnCollision(GameObject* other) override;

	void OnCollisionEnter(GameObject* other) override;

	void OnCollisionExit(GameObject* other) override;

private:


	void ApplyGlobalVariables();

private:
	Input* input_ = nullptr;

	float speed_ = 96.0f;

	float proximityThreshold_ = 10.0f;

	float maxDashDuration_ = 0.5f;

	float currentDashDuration_ = 0;

	bool isDashFinished_ = false;

	float maxChargeDuration_ = 0.16f;

	float currentChargeDuration_ = 0;

	bool isChargingFinished_ = false;

	float maxRecoveryDuration_ = 0.3f;

	float currentRecoveryDuration_ = 0;

	ParticleSystem* particleSystem_ = nullptr;
};

