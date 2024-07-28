#pragma once
#include "IPlayerState.h"

class PlayerStateDead : public IPlayerState
{
public:
	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	void OnCollision(GameObject* other);

	void OnCollisionEnter(GameObject* other);

	void OnCollisionExit(GameObject* other);

private:
	bool isDissolveEnable_ = false;

	float deathTimer_ = 0.0f;
};

