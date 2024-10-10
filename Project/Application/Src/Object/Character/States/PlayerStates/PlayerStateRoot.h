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

	void OnCollision(GameObject* other) override;

private:
	void SetIdleAnimationIfNotPlaying();

	void Move(const Vector3& inputValue, const float inputLength);

	void UpdateRotation(const Vector3& velocity);

	void UpdateAnimation(const Vector3& inputValue, bool isRunning);

	const std::string DetermineWalkingAnimation(const Vector3& inputValue) const;

	const std::string DetermineRunningAnimation(const Vector3& inputValue) const;

	void ProcessStateTransition();

private:
	//インプット
	Input* input_ = nullptr;

	//現在のアニメーションの名前
	std::string currentAnimationName_{};
};

