#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"

class PlayerStateDodge : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

private:
	//インプット
	Input* input_ = nullptr;

	//開始座標
	Vector3 startPosition_{};

	//目標座標
	Vector3 targetPosition_{};
};

