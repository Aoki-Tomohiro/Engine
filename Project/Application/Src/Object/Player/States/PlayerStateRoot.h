#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"
#include <string>

class PlayerStateRoot : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

private:
	void Move(const Vector3& inputValue, const float inputLength);

	void UpdateRotation(const Vector3& velocity);

	void UpdateAnimation(const Vector3& inputValue, bool isRunning);

	const std::string DetermineWalkingAnimation(const Vector3& inputValue) const;

	const std::string DetermineRunningAnimation(const Vector3& inputValue) const;

private:
	//インプット
	Input* input_ = nullptr;

	//現在のアニメーション
	std::string currentAnimation_ = "Idle";
};

