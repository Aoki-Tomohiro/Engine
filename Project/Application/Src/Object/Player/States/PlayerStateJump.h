#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Component/TransformComponent.h"

class PlayerStateJump : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

private:
	void AppliGlobalVariables();

private:
	Input* input_ = nullptr;

	float jumpFirstSpeed_ = 0.75f;

	float gravityAcceleration_ = 0.05f;
};
