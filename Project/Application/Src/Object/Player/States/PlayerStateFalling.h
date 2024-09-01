#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/Vector3.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"

class PlayerStateFalling : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;

private:
	void CheckAndPauseAnimation();

	void ApplyGravity();

	void CheckLanding();

	void HandleLanding();

	Vector3 GetInputValue();

private:
	//インプット
	Input* input_ = nullptr;

	//アニメーションの状態
	CombatAnimationState animationState_{};

	//速度
	Vector3 velocity_{};

	//着地フラグ
	bool isLanding_ = false;
};

