#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"

class PlayerStateChargedMagicAttack : public IPlayerState
{
public:
	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	void OnCollision(GameObject* other);

	void OnCollisionEnter(GameObject* other);

	void OnCollisionExit(GameObject* other);

private:
	Vector3 decelerationVector_{};
};

