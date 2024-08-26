#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"

class PlayerStateDodge : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

private:
	void PlayDodgeAnimation(const Vector3& inputValue);

	void PlayDirectionalDodgeAnimation(const Vector3& inputValue);

	Vector3 CalculateDodgeDistance(const Vector3& inputValue) const;

	Vector3 CalculateFallbackDistance() const;

	float CalculateEasingParameter() const;

	Vector3 InterpolatePosition(float easingParameter) const;

	void FinalizeDodge();

private:
	//インプット
	Input* input_ = nullptr;

	//開始座標
	Vector3 startPosition_{};

	//目標座標
	Vector3 targetPosition_{};
};

