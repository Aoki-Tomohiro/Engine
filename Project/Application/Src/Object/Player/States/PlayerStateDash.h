#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"

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

	float speed_ = 2.0f;

	float proximityThreshold_ = 10.0f;

	uint32_t maxDashDuration_ = 30;

	uint32_t currentDashDuration_ = 0;
};

