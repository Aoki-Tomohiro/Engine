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
	//Input
	Input* input_ = nullptr;

	//減速度
	Vector3 decelerationVector_{};

	//空中にいる状態かどうか
	bool isInAir_ = false;
};

