#pragma once
#include "IPlayerState.h"
#include "Engine/Math/Vector4.h"

class PlayerStateDeath : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;

private:
	Vector4 damagedSpriteColor_{};
};

