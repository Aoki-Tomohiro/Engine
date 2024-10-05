#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"

class PlayerStateDodge : public IPlayerState
{
public:
	//回避状態
	enum class DodgeState
	{
		kForward,
		kBackward,
	};

	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;

private:
	void SetupDodge(const std::string& animationName, const float animationSpeed);

	void UpdateAnimationPhase();

	void CalculateVelocity();

	void FinalizeDodge();

private:
	//インプット
	Input* input_ = nullptr;

	//入力値
	Vector3 inputValue_{};

	//速度
	Vector3 velocity_{};

	//アニメーションの状態
	CombatAnimationState animationState_{};

	//フェーズのインデックス
	int32_t phaseIndex_ = 0;

	//回避状態
	DodgeState dodgeState_ = DodgeState::kForward;
};

