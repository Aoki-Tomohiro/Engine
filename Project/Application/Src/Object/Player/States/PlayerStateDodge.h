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

	float dodgeSpeed_ = 0.4f;

	bool isBackFlip_ = false;

	int backFlipAnticipationTime = 10;

	int backFlipTime = 36;

	int backFlipParameter_ = 0;

	enum BackFlipState
	{
		kAnticipation,
		kBackFlip,
		kRecovery,
	};

	BackFlipState backFlipState_ = BackFlipState::kAnticipation;
};

