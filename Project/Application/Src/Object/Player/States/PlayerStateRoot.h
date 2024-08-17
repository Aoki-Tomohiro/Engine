#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"

class PlayerStateRoot : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

private:
	void UpdateWalkingAnimation(const Vector3& inputValue);

	void UpdateRunningAnimation(const Vector3& inputValue);

private:
	//Input
	Input* input_ = nullptr;
};

