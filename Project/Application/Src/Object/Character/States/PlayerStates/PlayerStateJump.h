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
	//Input
	Input* input_ = nullptr;

	//速度
	Vector3 velocity_{};

	//ボタンが押されている時間
	float buttonPressedTime_ = 0.0f;

	//同時押しと判定する時間
	const float kSimultaneousPressThreshold_ = 0.1f;
};

