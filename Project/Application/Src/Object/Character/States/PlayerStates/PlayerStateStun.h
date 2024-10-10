#pragma once
#include "IPlayerState.h"
#include "Engine/Math/Vector4.h"

class PlayerStateStun : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;
};

