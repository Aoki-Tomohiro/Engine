#include "ICharacterState.h"
#include "Engine/Components/Input/Input.h"
#include "Application/Src/Object/Character/BaseCharacter.h"

void ICharacterState::UpdateAnimationState()
{
	//アニメーターを取得
	AnimatorComponent* animator = character_->GetAnimator();

	//現在のアニメーション時間を取得
	float currentAnimationTime = animator->GetIsBlendingCompleted() ? animator->GetCurrentAnimationTime() : animator->GetNextAnimationTime();

	//アニメーション速度設定を更新
	UpdateAnimationSpeed(currentAnimationTime, animator);

	//アニメーションイベントを処理
	ProcessAnimationEvents(currentAnimationTime);
}

void ICharacterState::UpdateAnimationSpeed(const float currentAnimationTime, AnimatorComponent* animator)
{
	//全てのアニメーション時間の設定を適用
	if (speedConfigIndex_ < static_cast<int32_t>(animationController_->animationSpeedConfigs.size()))
	{
		//現在のアニメーション時間が設定した持続時間を超えた場合
		if (speedConfigIndex_ == -1 || currentAnimationTime > animationController_->animationSpeedConfigs[speedConfigIndex_].duration)
		{
			//次の設定に移動
			speedConfigIndex_++;
			//アニメーションの速度を設定
			animator->SetCurrentAnimationSpeed(animationController_->animationSpeedConfigs[speedConfigIndex_].animationSpeed);
			animator->SetNextAnimationSpeed(animationController_->animationSpeedConfigs[speedConfigIndex_].animationSpeed);
		}
	}
}

void ICharacterState::ProcessAnimationEvents(const float currentAnimationTime)
{
	//全てのアニメーションイベントを処理
	for (const std::shared_ptr<AnimationEvent>& animationEvent : animationController_->animationEvents)
	{
		//現在のアニメーション時間がイベントの開始時間と終了時間の間にある場合
		if (animationEvent->startEventTime < currentAnimationTime && animationEvent->endEventTime >= currentAnimationTime)
		{
			//アニメーションイベントを処理
			ProcessAnimationEvent(animationEvent.get(), currentAnimationTime);
		}
		else
		{
			//イベントが終了している場合は処理データをリセット
			ResetProcessedData(animationEvent->eventType);
		}
	}
}

void ICharacterState::ProcessAnimationEvent(const AnimationEvent* animationEvent, const float animationTime)
{
	//アニメーションイベントの種類に応じて処理を分岐
	switch (animationEvent->eventType)
	{
	case EventType::kMovement:
		//移動イベントの処理
		ProcessMovementEvent(static_cast<const MovementEvent*>(animationEvent), animationTime);
		break;
	case EventType::kAttack:
		//攻撃イベントの処理
		ProcessAttackEvent(static_cast<const AttackEvent*>(animationEvent));
		break;
	}
}

void ICharacterState::ProcessMovementEvent(const MovementEvent* movementEvent, const float animationTime)
{
	//移動イベントの種類に応じて処理を分岐
	switch (movementEvent->movementType)
	{
	case MovementType::kVelocity:
		//速度移動イベントの処理
		ProcessVelocityMovementEvent(static_cast<const VelocityMovementEvent*>(movementEvent));
		break;
	case MovementType::kEasing:
		//イージング移動イベントの処理
		ProcessEasingMovementEvent(static_cast<const EasingMovementEvent*>(movementEvent), animationTime);
		break;
	}
}

void ICharacterState::ProcessVelocityMovementEvent(const VelocityMovementEvent* velocityMovementEvent)
{
	//速度移動イベントがまだアクティブでない場合の初期化処理
	if (!processedVelocityData_.isActive)
	{
		//速度移動イベントの初期化
		InitializeVelocityMovement(velocityMovementEvent);
	}

	//キャラクターを移動させる
	character_->Move(processedVelocityData_.velocity);
}

void ICharacterState::ProcessEasingMovementEvent(const EasingMovementEvent* easingMovementEvent, const float animationTime)
{
	//イージング移動イベントがまだアクティブでない場合の初期化処理
	if (!processedEasingData_.isActive)
	{
		//イージング移動イベントの初期化
		InitializeEasingMovementEvent(easingMovementEvent);
	}

	//イージングパラメータを計算
	float easingParameter = std::min<float>(1.0f, (animationTime - easingMovementEvent->startEventTime) / (easingMovementEvent->endEventTime - easingMovementEvent->startEventTime));

	//キャラクターの位置を更新
	character_->SetPosition(Mathf::Lerp(processedEasingData_.startPosition, processedEasingData_.targetPosition, easingParameter));
}

void ICharacterState::InitializeAttackEvent(const AttackEvent* attackEvent)
{
	//アクティブ状態にする
	processedAttackData_.isActive = true;
	//攻撃パラメータを武器に適用
	character_->ApplyParametersToWeapon(attackEvent);
}

void ICharacterState::ProcessAttackEvent(const AttackEvent* attackEvent)
{
	//攻撃イベントがまだアクティブでない場合の初期化処理
	if (!processedAttackData_.isActive)
	{
		//攻撃イベントの初期化
		InitializeAttackEvent(attackEvent);
	}

	//現在のヒットカウントが最大ヒットカウントに達していない場合
	if (processedAttackData_.currentHitCount < attackEvent->attackParameters.maxHitCount)
	{
		//武器を取得
		Weapon* weapon = character_->GetWeapon();

		//攻撃タイマーを進める
		processedAttackData_.elapsedAttackTime += GameTimer::GetDeltaTime();

		//ヒット間隔に基づいて攻撃判定を設定
		if (processedAttackData_.elapsedAttackTime > attackEvent->attackParameters.hitInterval)
		{
			weapon->SetIsAttack(true); //攻撃状態を有効にする
			processedAttackData_.elapsedAttackTime = 0.0f; //攻撃タイマーをリセット
		}

		//攻撃が敵に当たったかどうかをチェック
		if (weapon->GetIsHit())
		{
			processedAttackData_.currentHitCount++; //ヒットカウントを増やす
			weapon->SetIsAttack(false); //攻撃状態を無効にする
		}
	}
}

void ICharacterState::ResetProcessedData(const EventType eventType)
{
	//イベントタイプに応じたリセット処理
	switch (eventType)
	{
	case EventType::kMovement:
		//移動イベントのデータをリセット
		processedVelocityData_.isActive = false;
		processedEasingData_.isActive = false;
		break;
	case EventType::kAttack:
		//攻撃イベントのデータをリセット
		processedAttackData_.isActive = false;
		break;
	}
}