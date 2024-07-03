#pragma once
#include "IPlayerState.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GlobalVariables.h"

class PlayerStateDash : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

private:
	void ApplyGlobalVariables();

private:
	Input* input_ = nullptr;

	float speed_ = 1.6f;

	float proximityThreshold_ = 10.0f;

	uint32_t maxDashDuration_ = 30;

	uint32_t currentDashDuration_ = 0;

	bool isDashFinished_ = false;

	uint32_t maxChargeDuration_ = 10;

	uint32_t currentChargeDuration_ = 0;

	bool isChargingFinished_ = false;

	uint32_t maxRecoveryDuration_ = 20;

	uint32_t currentRecoveryDuration_ = 0;
};

