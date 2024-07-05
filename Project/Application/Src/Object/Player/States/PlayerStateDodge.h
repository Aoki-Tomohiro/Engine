#pragma once
#include "IPlayerState.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GlobalVariables.h"

class PlayerStateDodge : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

private:
	void ApplyGlobalVariables();

private:
	Input* input_ = nullptr;

	float dodgeSpeed_ = 24.0f;

	bool isBackFlip_ = false;

	float backFlipAnticipationTime = 0.2f;

	float backFlipTime = 0.6f;

	float backFlipParameter_ = 0;

	enum BackFlipState
	{
		kAnticipation,
		kBackFlip,
		kRecovery,
	};

	BackFlipState backFlipState_ = BackFlipState::kAnticipation;
};

