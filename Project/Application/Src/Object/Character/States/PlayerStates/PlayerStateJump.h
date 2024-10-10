#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"

class PlayerStateJump : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;

private:
	void CreateLandingParticles();

private:
	//Input
	Input* input_ = nullptr;

	//速度
	Vector3 velocity_{};
};

