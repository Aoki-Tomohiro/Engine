#include "ICharacterState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/BaseCharacter.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"

void ICharacterState::SetAnimationControllerAndPlayAnimation(const std::string& animationName)
{
	//アニメーションを再生
	character_->GetAnimator()->PlayAnimation(animationName, 1.0f, false);

	//アニメーションコントローラーを取得
	animationController_ = &character_->GetEditorManager()->GetCombatAnimationEditor()->GetAnimationController(character_->GetName(), animationName);

	//アニメーション速度設定のインデックスをリセット
	speedConfigIndex_ = -1;

	//各パラメーターを初期化
	InitializeProcessedData();
}

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

const bool ICharacterState::CheckAndTransitionBufferedAction()
{
	//先行入力があるかを確認
	for (ProcessedBufferedActionData& bufferedActionData : processedBufferedActionDatas_)
	{
		//先行入力があった場合
		if (!bufferedActionData.bufferedActionName.empty())
		{
			//アニメーション補正のフラグが立っている場合は補正を掛ける
			if (bufferedActionData.isAnimationCorrectionActive)
			{
				character_->CorrectPositionAfterAnimation();
			}

			//状態遷移
			character_->ChangeState(bufferedActionData.bufferedActionName);

			//状態遷移が行われた
			return true;
		}
	}

	return false;
}

void ICharacterState::InitializeProcessedData()
{
	//各イベントタイプに対する処理データを初期化
	processedVelocityDatas_.resize(animationController_->GetAnimationEventCount(EventType::kMovement));
	processedEasingDatas_.resize(animationController_->GetAnimationEventCount(EventType::kMovement));
	processedAttackDatas_.resize(animationController_->GetAnimationEventCount(EventType::kAttack));
	processedCancelDatas_.resize(animationController_->GetAnimationEventCount(EventType::kCancel));
	processedBufferedActionDatas_.resize(animationController_->GetAnimationEventCount(EventType::kBufferedAction));
}

void ICharacterState::UpdateAnimationSpeed(const float currentAnimationTime, AnimatorComponent* animator)
{
	//アニメーション速度設定を更新
	if (speedConfigIndex_ < static_cast<int32_t>(animationController_->animationSpeedConfigs.size() - 1))
	{
		//現在のアニメーション時間が設定した持続時間を超えた場合
		if (speedConfigIndex_ == -1 || currentAnimationTime > animationController_->animationSpeedConfigs[speedConfigIndex_].duration)
		{
			speedConfigIndex_++;
			animator->SetCurrentAnimationSpeed(animationController_->animationSpeedConfigs[speedConfigIndex_].animationSpeed);
			animator->SetNextAnimationSpeed(animationController_->animationSpeedConfigs[speedConfigIndex_].animationSpeed);
		}
	}
}

void ICharacterState::ProcessAnimationEvents(const float currentAnimationTime)
{
	//アニメーションイベントのインデックス
	int32_t animationEventIndex[static_cast<int>(EventType::kMaxEvent)]{};

	//全てのアニメーションイベントを処理
	for (const std::shared_ptr<AnimationEvent>& animationEvent : animationController_->animationEvents)
	{
		//現在のアニメーション時間にイベントが発生しているか確認
		if (animationEvent->startEventTime <= currentAnimationTime && animationEvent->endEventTime > currentAnimationTime)
		{
			//アニメーションイベントを処理
			ProcessAnimationEvent(animationEvent.get(), currentAnimationTime, animationEventIndex[static_cast<int>(animationEvent->eventType)]);
		}
		else
		{
			//イベントが終了している場合は処理データをリセット
			ResetProcessedData(animationEvent->eventType, animationEventIndex[static_cast<int>(animationEvent->eventType)]);
		}

		//イベントインデックスを進める
		animationEventIndex[static_cast<int>(animationEvent->eventType)]++;
	}
}

void ICharacterState::ProcessAnimationEvent(const AnimationEvent* animationEvent, const float animationTime, const int32_t animationEventIndex)
{
	//アニメーションイベントの種類に応じて処理を分岐
	switch (animationEvent->eventType)
	{
	case EventType::kMovement:
		//移動イベントの処理
		ProcessMovementEvent(static_cast<const MovementEvent*>(animationEvent), animationTime, animationEventIndex);
		break;
	case EventType::kAttack:
		//攻撃イベントの処理
		ProcessAttackEvent(static_cast<const AttackEvent*>(animationEvent), animationEventIndex);
		break;
	case EventType::kCancel:
		//キャンセルイベントの処理
		ProcessCancelEvent(static_cast<const CancelEvent*>(animationEvent), animationEventIndex);
		break;
	case EventType::kBufferedAction:
		//先行入力イベントの処理
		ProcessBufferedActionEvent(static_cast<const BufferedActionEvent*>(animationEvent), animationEventIndex);
		break;
	}
}

void ICharacterState::ProcessMovementEvent(const MovementEvent* movementEvent, const float animationTime, const int32_t animationEventIndex)
{
	//移動イベントの種類に応じて処理を分岐
	switch (movementEvent->movementType)
	{
	case MovementType::kVelocity:
		//速度移動イベントの処理
		ProcessVelocityMovementEvent(static_cast<const VelocityMovementEvent*>(movementEvent), animationEventIndex);
		break;
	case MovementType::kEasing:
		//イージング移動イベントの処理
		ProcessEasingMovementEvent(static_cast<const EasingMovementEvent*>(movementEvent), animationTime, animationEventIndex);
		break;
	}
}

void ICharacterState::ProcessVelocityMovementEvent(const VelocityMovementEvent* velocityMovementEvent, const int32_t animationEventIndex)
{
	//速度移動イベントがまだアクティブでない場合の初期化処理
	if (!processedVelocityDatas_[animationEventIndex].isActive)
	{
		//速度移動イベントの初期化
		InitializeVelocityMovement(velocityMovementEvent, animationEventIndex);
	}

	//キャラクターを移動させる
	character_->Move(processedVelocityDatas_[animationEventIndex].velocity);

	//敵と近距離で速度をゼロに設定
	if (velocityMovementEvent->isProximityStopEnabled && IsOpponentInProximity(kProximityDistance))
	{
		processedVelocityDatas_[animationEventIndex].velocity = { 0.0f, 0.0f, 0.0f };
	}
}

void ICharacterState::ProcessEasingMovementEvent(const EasingMovementEvent* easingMovementEvent, const float animationTime, const int32_t animationEventIndex)
{
	//イージング移動イベントがまだアクティブでない場合の初期化処理
	if (!processedEasingDatas_[animationEventIndex].isActive)
	{
		//イージング移動イベントの初期化
		InitializeEasingMovementEvent(easingMovementEvent, animationEventIndex);
	}

	//イージングパラメータを計算
	float easingParameter = std::min<float>(1.0f, (animationTime - easingMovementEvent->startEventTime) / (easingMovementEvent->endEventTime - easingMovementEvent->startEventTime));

	//キャラクターの位置を更新
	character_->SetPosition(Mathf::Lerp(processedEasingDatas_[animationEventIndex].startPosition, processedEasingDatas_[animationEventIndex].targetPosition, easingParameter));

	//敵と近距離でイージング終了
	if (easingMovementEvent->isProximityStopEnabled && IsOpponentInProximity(kProximityDistance))
	{
		processedEasingDatas_[animationEventIndex].startPosition = character_->GetPosition();
		processedEasingDatas_[animationEventIndex].targetPosition = character_->GetPosition();
	}
}

void ICharacterState::InitializeAttackEvent(const AttackEvent* attackEvent, const int32_t animationEventIndex)
{
	//アクティブ状態にする
	processedAttackDatas_[animationEventIndex].isActive = true;
	//攻撃パラメータを武器に適用
	character_->ApplyParametersToWeapon(attackEvent);
}

void ICharacterState::ProcessAttackEvent(const AttackEvent* attackEvent, const int32_t animationEventIndex)
{
	//攻撃イベントがまだアクティブでない場合の初期化処理
	if (!processedAttackDatas_[animationEventIndex].isActive)
	{
		//攻撃イベントの初期化
		InitializeAttackEvent(attackEvent, animationEventIndex);
	}

	//現在のヒットカウントが最大ヒットカウントに達していない場合
	if (processedAttackDatas_[animationEventIndex].currentHitCount < attackEvent->attackParameters.maxHitCount)
	{
		//武器を取得
		Weapon* weapon = character_->GetWeapon();

		//攻撃タイマーを進める
		processedAttackDatas_[animationEventIndex].elapsedAttackTime += GameTimer::GetDeltaTime();

		//ヒット間隔に基づいて攻撃判定を設定
		if (processedAttackDatas_[animationEventIndex].elapsedAttackTime > attackEvent->attackParameters.hitInterval)
		{
			weapon->SetIsAttack(true); //攻撃状態を有効にする
			processedAttackDatas_[animationEventIndex].elapsedAttackTime = 0.0f; //攻撃タイマーをリセット
		}

		//攻撃が敵に当たったかどうかをチェック
		if (weapon->GetIsHit())
		{
			processedAttackDatas_[animationEventIndex].currentHitCount++; //ヒットカウントを増やす
			weapon->SetIsAttack(false); //攻撃状態を無効にする
		}
	}
}

void ICharacterState::ProcessCancelEvent(const CancelEvent* cancelEvent, const int32_t animationEventIndex)
{
	//速度移動イベントがまだアクティブでない場合の初期化処理
	if (!processedCancelDatas_[animationEventIndex].isActive)
	{
		processedCancelDatas_[animationEventIndex].isActive = true;
	}

	//先行入力があった場合その状態に遷移
	CheckAndTransitionBufferedAction();

	//キャンセルアクションを処理
	HandleCancelAction(cancelEvent, animationEventIndex);
}

void ICharacterState::ProcessBufferedActionEvent(const BufferedActionEvent* bufferedActionEvent, const int32_t animationEventIndex)
{
	//先行入力イベントがまだアクティブでない場合の初期化処理
	if (!processedBufferedActionDatas_[animationEventIndex].isActive)
	{
		processedBufferedActionDatas_[animationEventIndex].isActive = true;
		processedBufferedActionDatas_[animationEventIndex].isAnimationCorrectionActive = bufferedActionEvent->isAnimationCorrectionActive;
	}

	//先行入力アクションを処理
	HandleBufferedAction(bufferedActionEvent, animationEventIndex);
}

const bool ICharacterState::IsOpponentInProximity(const float proximityDistance) const
{
	//ゲームオブジェクトマネージャーを取得
	GameObjectManager* gameObjectManager = GameObjectManager::GetInstance();

	//自分の座標を取得
	Vector3 myPosition = character_->GetJointWorldPosition("mixamorig:Hips");

	//相手の座標を取得
	Vector3 otherPosition = (character_->GetName() == "Player") ? gameObjectManager->GetGameObject<Enemy>("Enemy")->GetJointWorldPosition("mixamorig:Hips") :
		gameObjectManager->GetGameObject<Player>("Player")->GetJointWorldPosition("mixamorig:Hips");

	return Mathf::Length(otherPosition - myPosition) < proximityDistance;
}

void ICharacterState::ResetProcessedData(const EventType eventType, const int32_t animationEventIndex)
{
	//各イベントタイプに応じて処理データをリセット
	switch (eventType)
	{
	case EventType::kMovement:
		processedVelocityDatas_[animationEventIndex].isActive = false;
		processedEasingDatas_[animationEventIndex].isActive = false;
		break;
	case EventType::kAttack:
		processedAttackDatas_[animationEventIndex].isActive = false;
		character_->GetWeapon()->SetIsAttack(false);
		break;
	case EventType::kBufferedAction:
		processedBufferedActionDatas_[animationEventIndex].isActive = false;
		break;
	}
}