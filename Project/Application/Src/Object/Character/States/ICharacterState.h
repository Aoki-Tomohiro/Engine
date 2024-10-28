#pragma once
#include "Application/Src/Object/Editors/CombatAnimationEditor/CombatAnimationEditor.h"

class BaseCharacter;

class ICharacterState
{
public:
	virtual ~ICharacterState() = default;

	virtual void Initialize() = 0;

	virtual void Update() = 0;

protected:
	void ProcessAnimationEvents();

private:
	void ProcessAnimationEvent(AnimationEvent* animationEvent, const float animationTime);

	void ProcessMovementEvent(MovementEvent* movementEvent, const float animationTime);

	void ProcessVelocityMovementEvent(VelocityMovementEvent* velocityMovementEvent);

	void ProcessEasingMovementEvent(EasingMovementEvent* easingMovementEvent, const float animationTime);

	void ProcessAttackEvent(AttackEvent* attackEvent);

protected:
	BaseCharacter* character_ = nullptr;

	const std::vector<std::unique_ptr<AnimationEvent>>& animationEvents_{};

	Vector3 velocity_{};

	Vector3 startPosition_{};

	Vector3 targetPosition_{};
};