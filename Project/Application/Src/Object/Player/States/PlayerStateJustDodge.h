#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"

class PlayerStateJustDodge : public IPlayerState
{
public:
	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	void OnCollision(GameObject* other);

	void OnCollisionEnter(GameObject* other);

	void OnCollisionExit(GameObject* other);

private:
	Input* input_ = nullptr;

	float dodgeParameter_ = 0.0f;

	float justDodgeDuration_ = 0.16f;

	float recoveryDuration_ = 2.0f;
};

