#pragma once
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"

class GameObject;

class ICharacterState
{
public:
	virtual void Initialize() = 0;

	virtual void Update() = 0;

	virtual void OnCollision(GameObject* gameObject) = 0;

protected:
	//アニメーションの状態
	CombatAnimationState animationState_{};

	//フェーズのインデックス
	int32_t phaseIndex_ = 0;
};

