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
	//Input
	Input* input_ = nullptr;

	//始点座標
	Vector3 startPosition_{};

	//目標座標
	Vector3 targetPosition_{};

	//ジャスト回避のタイマー
	float justDodgeTimer_ = 0.0f;           
};

