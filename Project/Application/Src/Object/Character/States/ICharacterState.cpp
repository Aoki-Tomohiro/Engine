#include "ICharacterState.h"
#include "Application/Src/Object/Character/BaseCharacter.h"

void ICharacterState::ProcessAnimationEvents()
{
	//アニメーターを取得
	AnimatorComponent* animator = character_->GetAnimator();

	//アニメーションの時間を取得
	float currentAnimationTime = animator->GetIsBlendingCompleted() ? animator->GetCurrentAnimationTime() : animator->GetNextAnimationTime();

	//全てのアニメーションイベントの処理
	for (const std::unique_ptr<ActionParameters>& animationEvent : animationEvents_)
	{
		//現在のアニメーション時間がイベント時間内入っていた場合
		if (animationEvent->startEventTime < currentAnimationTime && animationEvent->endEventTime >= currentAnimationTime)
		{
			ProcessAnimationEvent(animationEvent.get(), currentAnimationTime);
		}
		else
		{
			animationEvent->isActive = false;
		}
	}
}

void ICharacterState::ProcessAnimationEvent(ActionParameters* animationEvent, const float animationTime)
{
	switch (animationEvent->parameterType)
	{
	case ParameterType::kMovement:
		ProcessMovementEvent(dynamic_cast<MovementEvent*>(animationEvent), animationTime);
		break;
	case ParameterType::kAttack:
		ProcessAttackEvent(dynamic_cast<ActionParameters*>(animationEvent));
		break;
	}
}

void ICharacterState::ProcessMovementEvent(MovementEvent* movementEvent, const float animationTime)
{
	switch (movementEvent->movementType)
	{
	case MovementType::kVelocity:
		ProcessVelocityMovementEvent(dynamic_cast<VelocityMovementEvent*>(movementEvent));
		break;
	case MovementType::kEasing:
		ProcessEasingMovementEvent(dynamic_cast<EasingMovementEvent*>(movementEvent), animationTime);
		break;
	}
}

void ICharacterState::ProcessVelocityMovementEvent(VelocityMovementEvent* velocityMovementEvent)
{
	if (!velocityMovementEvent->isActive)
	{
		velocityMovementEvent->isActive = true;
		velocity_ = Mathf::RotateVector(velocityMovementEvent->velocity, character_->GetQuaternion());
	}

	character_->Move(velocity_);
}

void ICharacterState::ProcessEasingMovementEvent(EasingMovementEvent* easingMovementEvent, const float animationTime)
{
	if (!easingMovementEvent->isActive)
	{
		easingMovementEvent->isActive = true;
		startPosition_ = character_->GetPosition();
		targetPosition_ = startPosition_ + Mathf::RotateVector(easingMovementEvent->targetPosition, character_->GetQuaternion());
	}

	float easingParameter = (animationTime - easingMovementEvent->startEventTime) / (easingMovementEvent->endEventTime - easingMovementEvent->startEventTime);
	easingParameter = std::clamp(easingParameter, 0.0f, 1.0f);
	character_->SetPosition(Mathf::Lerp(startPosition_, targetPosition_, easingParameter));
}

void ICharacterState::ProcessAttackEvent(ActionParameters* attackEvent)
{
	if (!attackEvent->isActive)
	{
		attackEvent->isActive = true;
		character_->ApplyParametersToWeapon(attackEvent);
	}

	//武器を取得
	Weapon* weapon = character_->GetWeapon();
}