#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Component/TransformComponent.h"

class PlayerStateDodge : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

private:
	void AppliGlobalVariables();

private:
	Input* input_ = nullptr;

	float dodgeSpeed_ = 1.0f;

	uint32_t dodgeTime_ = 10;

	uint32_t dodgeTimer_ = 0;

	float rotationAngle_ = 0.2f;
};

