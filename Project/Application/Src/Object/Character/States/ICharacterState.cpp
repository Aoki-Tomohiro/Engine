#include "ICharacterState.h"
#include "Application/Src/Object/Character/BaseCharacter.h"

void ICharacterState::ProcessAnimationEvents()
{
	//アニメーターを取得
	AnimatorComponent* animator = character_->GetAnimator();

	//アニメーションの時間を取得
	float currentAnimationTime = animator->GetIsBlendingCompleted() ? animator->GetCurrentAnimationTime() : animator->GetNextAnimationTime();

	//全てのアニメーションイベントを処理
	for (const std::unique_ptr<AnimationEvent>& animationEvent : animationEvents_)
	{
		//現在のアニメーション時間がイベントの開始時間と終了時間の間にある場合
		if (animationEvent->startEventTime < currentAnimationTime && animationEvent->endEventTime >= currentAnimationTime)
		{
			//アニメーションイベントを実行
			ProcessAnimationEvent(animationEvent.get(), currentAnimationTime);
		}
	}
}

const bool ICharacterState::IsAnimationEventActive(const std::string& eventName) const
{
	//指定されたイベント名を持つアニメーションイベントを検索
	for (const std::unique_ptr<AnimationEvent>& animationEvent : animationEvents_)
	{
		//同じ名前のアニメーションイベントが存在したらアクティブかどうかを返す
		if (animationEvent->eventName == eventName)
		{
			return animationEvent->isActive;
		}
	}
	//イベントが見つからなかった場合はfalseを返す
	return false;
}

void ICharacterState::ProcessAnimationEvent(AnimationEvent* animationEvent, const float animationTime)
{
	//アニメーションイベントの種類に応じて処理を分岐
	switch (animationEvent->eventType)
	{
	case EventType::kMovement:
		ProcessMovementEvent(dynamic_cast<MovementEvent*>(animationEvent), animationTime);
		break;
	case EventType::kAttack:
		ProcessAttackEvent(dynamic_cast<AttackEvent*>(animationEvent));
		break;
	}
}

void ICharacterState::ProcessMovementEvent(MovementEvent* movementEvent, const float animationTime)
{
	//移動イベントの種類に応じて処理を分岐
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
	//速度移動イベントがまだアクティブでない場合の初期化処理
	if (!velocityMovementEvent->isActive)
	{
		velocityMovementEvent->isActive = true;
		//キャラクターの向きに合わせて速度ベクトルを回転させる
		velocityMovementEvent->velocity = Mathf::RotateVector(velocityMovementEvent->velocity, character_->GetQuaternion());
	}

	//キャラクターを移動させる
	character_->Move(velocityMovementEvent->velocity);
}

void ICharacterState::ProcessEasingMovementEvent(EasingMovementEvent* easingMovementEvent, const float animationTime)
{
	//イージング移動イベントがまだアクティブでない場合の初期化処理
	if (!easingMovementEvent->isActive)
	{
		easingMovementEvent->isActive = true;
		//キャラクターの現在の位置を開始位置として設定
		easingMovementEvent->startPosition = character_->GetPosition();
		//目標位置を計算
		easingMovementEvent->targetPosition = easingMovementEvent->startPosition + Mathf::RotateVector(easingMovementEvent->targetPosition, character_->GetQuaternion());
	}

	//イージングパラメータを計算
	float easingParameter = (animationTime - easingMovementEvent->startEventTime) / (easingMovementEvent->endEventTime - easingMovementEvent->startEventTime);
	easingParameter = std::clamp(easingParameter, 0.0f, 1.0f);

	//キャラクターの位置を更新
	character_->SetPosition(Mathf::Lerp(easingMovementEvent->startPosition, easingMovementEvent->targetPosition, easingParameter));
}

void ICharacterState::ProcessAttackEvent(AttackEvent* attackEvent)
{
	//攻撃イベントがまだアクティブでない場合の初期化処理
	if (!attackEvent->isActive)
	{
		attackEvent->isActive = true;
		//ノックバックパラメータを回転させる
		attackEvent->knockbackParameters.velocity = Mathf::RotateVector(attackEvent->knockbackParameters.velocity, character_->GetQuaternion());
		attackEvent->knockbackParameters.acceleration = Mathf::RotateVector(attackEvent->knockbackParameters.acceleration, character_->GetQuaternion());
		//攻撃パラメータを武器に適用
		character_->ApplyParametersToWeapon(attackEvent);
	}

	//武器を取得
	Weapon* weapon = character_->GetWeapon();

	//現在のヒットカウントが最大ヒットカウントに達していない場合
	if (attackEvent->attackParameters.currentHitCount < attackEvent->attackParameters.maxHitCount)
	{
		//攻撃タイマーを進める
		attackEvent->attackParameters.elapsedAttackTime += GameTimer::GetDeltaTime();

		//ヒット間隔に基づいて攻撃判定を設定
		if (attackEvent->attackParameters.elapsedAttackTime > attackEvent->attackParameters.hitInterval)
		{
			weapon->SetIsAttack(true); //攻撃状態を有効にする
			attackEvent->attackParameters.elapsedAttackTime = 0.0f; //攻撃タイマーをリセット
		}

		//攻撃が敵に当たったかどうかをチェック
		if (weapon->GetIsHit())
		{
			attackEvent->attackParameters.currentHitCount++; //ヒットカウントを増やす
			weapon->SetIsAttack(false); //攻撃状態を無効にする
		}
	}
}