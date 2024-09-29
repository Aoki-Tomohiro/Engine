#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"

class PlayerStateDodge : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;

private:
	void SetupDodge(const std::string& animationName, const float animationSpeed);

	void UpdateAnimationPhase();

	void FinalizeDodge();

private:
	//インプット
	Input* input_ = nullptr;

	//速度
	Vector3 velocity_{};

	//アニメーションの状態
	CombatAnimationState animationState_{};

	//フェーズのインデックス
	int32_t phaseIndex_ = 0;
};

