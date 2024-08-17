#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/AnimationStateManager/AnimationStateManager.h"

class PlayerStateAttackBackhand : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

private:
	//Input
	Input* input_ = nullptr;

	//速度
	Vector3 velocity_{};

	//アニメーションの状態
	AnimationState state_{};

	//フェーズの名前
	std::string phaseName_{};

	//ヒットタイマー
	float hitTimer_ = 0.0f;

	//ヒットカウント
	int32_t hitCount_ = 0;
};

