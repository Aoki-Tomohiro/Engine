/**
 * @file AbstractCharacterState.cpp
 * @brief キャラクターの状態の基底クラスを管理するファイル
 * @author 青木智滉
 * @date
 */

#include "AbstractCharacterState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/BaseCharacter.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"

void AbstractCharacterState::SetAnimationControllerAndPlayAnimation(const std::string& animationName, const bool isLoop)
{
	//アニメーションコントローラーを取得
	animationController_ = &character_->GetEditorManager()->GetCombatAnimationEditor()->GetAnimationController(character_->GetName(), animationName);

	//アニメーションを再生
	character_->GetAnimator()->PlayAnimation(animationName, 1.0f, isLoop, animationController_->inPlaceAxis);

	//アニメーション速度設定のインデックスをリセット
	speedConfigIndex_ = -1;

	//武器のトレイルをなくす
	character_->GetWeapon()->SetIsTrailActive(false);

	//武器の判定をなくす
	character_->GetWeapon()->SetIsAttack(false);

	//ジャスト回避ウィンドウをリセット
	character_->SetIsVulnerableToPerfectDodge(false);

	//各パラメーターを初期化
	InitializeProcessedData();
}

void AbstractCharacterState::UpdateAnimationState()
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

void AbstractCharacterState::HandleStateTransition()
{
	//先行入力があった場合はその状態に遷移して処理を飛ばす
	if (CheckAndTransitionBufferedAction())
	{
		return;
	}

	//状態遷移処理
	HandleStateTransitionInternal();
}

bool AbstractCharacterState::IsOpponentInProximity() const
{
	//ゲームオブジェクトマネージャーを取得
	GameObjectManager* gameObjectManager = GameObjectManager::GetInstance();

	//自分の座標を取得
	Vector3 myPosition = character_->GetJointWorldPosition("mixamorig:Hips");

	//相手の座標を取得
	Vector3 otherPosition = (character_->GetName() == "Player") ? gameObjectManager->GetGameObject<Enemy>("Enemy")->GetJointWorldPosition("mixamorig:Hips") :
		gameObjectManager->GetGameObject<Player>("Player")->GetJointWorldPosition("mixamorig:Hips");

	//差分ベクトルを計算
	Vector3 diff = otherPosition - myPosition;

	//水平方向の距離が知りたいのでY軸は0にする
	diff.y = 0.0f;

	return Mathf::Length(diff) < kProximityDistance;
}

bool AbstractCharacterState::HasStateTransitioned(const std::string& actionName) const
{
	//全てのアニメーションキャンセルを確認
	for (const ProcessedCancelData& cancelData : processedCancelDatas_)
	{
		//指定されたアクション名と一致しない場合はスキップ
		if (!actionName.empty() && actionName != cancelData.cancelActionName) continue;

		//アニメーションキャンセルが行われていた場合
		if (cancelData.isCanceled) return true;
	}

	//全ての先行入力を確認
	for (const ProcessedBufferedActionData& bufferedActionData : processedBufferedActionDatas_)
	{
		//指定されたアクション名と一致しない場合はスキップ
		if (!actionName.empty() && actionName != bufferedActionData.bufferedActionName) continue;

		//先行入力がアクティブである場合
		if (bufferedActionData.isTransitioned) return true;
	}

	//全てのQTEを確認
	for (const ProcessedQTEData& qteData : processedQTEDatas_)
	{
		//指定されたアクション名と一致しない場合はスキップ
		if (!actionName.empty() && actionName != qteData.qteActionName) continue;

		//QTEが成功した場合
		if (qteData.isSuccess) return true;
	}

	return false;
}

bool AbstractCharacterState::CheckAndTransitionBufferedAction(const std::string& actionName)
{
	//次の状態の名前
	std::string nextStateName{};

	//先行入力のインデックス
	int32_t actionIndex = 0;

	//先行入力があるかを確認
	for (int32_t i = 0; i < processedBufferedActionDatas_.size(); ++i)
	{
		//先行入力がない場合は飛ばす
		if (processedBufferedActionDatas_[i].bufferedActionName.empty()) continue;

		//名前の指定があった場合一致する名前の先行入力でなければ飛ばす
		if (!actionName.empty() && processedBufferedActionDatas_[i].bufferedActionName != actionName) continue;

		//先行入力が存在していた場合は次の状態に設定
		nextStateName = processedBufferedActionDatas_[i].bufferedActionName;

		//インデックスを保存
		actionIndex = i;

		//ループを抜ける
		break;
	}

	//先行入力が存在していて入力されていたらフラグを立てて状態遷移させる
	if (!nextStateName.empty() && character_->ChangeState(nextStateName))
	{
		//状態遷移が行われた
		processedBufferedActionDatas_[actionIndex].isTransitioned = true;
		return true;
	}

	//遷移が行われていなければfalseを返す
	return false;
}

void AbstractCharacterState::InitializeRotationEvent(const int32_t animationEventIndex)
{
	//アクティブ状態にする
	processedRotationDatas_[animationEventIndex].isActive = true;
}

void AbstractCharacterState::InitializeAttackEvent(const AttackEvent* attackEvent, const int32_t animationEventIndex)
{
	//アクティブ状態にする
	processedAttackDatas_[animationEventIndex].isActive = true;
	//武器の軌跡を有効にする
	character_->GetWeapon()->SetIsTrailActive(true);
	//攻撃パラメータを武器に適用
	character_->ApplyParametersToWeapon(attackEvent);
	//ジャスト回避ウィンドウをリセット
	character_->SetIsVulnerableToPerfectDodge(false);
}

void AbstractCharacterState::InitializeEffectEvent(const EffectEvent* effectEvent, const int32_t animationEventIndex)
{
	//アクティブ状態にする
	processedEffectDatas_[animationEventIndex].isActive = true;

	//ポストエフェクトのタイプを設定
	processedEffectDatas_[animationEventIndex].postEffectType = effectEvent->postEffectType;

	//エミッターの座標を設定
	UpdateEmitterPosition(effectEvent, animationEventIndex);

	//アクション開始時にトリガーする状態の場合はエフェクトを適用
	if (effectEvent->trigger == EventTrigger::kActionStart)
	{
		ApplyEffect(effectEvent, processedEffectDatas_[animationEventIndex].emitterPosition);
	}
}

void AbstractCharacterState::InitializeCameraAnimationEvent(const CameraAnimationEvent* cameraAnimationEvent, const int32_t animationEventIndex)
{
	//アクティブ状態にする
	processedCameraAnimationDatas_[animationEventIndex].isActive = true;

	//アクション開始時にトリガーする状態の場合はカメラアニメーションを再生
	if (cameraAnimationEvent->trigger == EventTrigger::kActionStart)
	{
		processedCameraAnimationDatas_[animationEventIndex].isAnimationStarted = true;
		character_->GetCameraController()->PlayAnimation(cameraAnimationEvent->animationName, cameraAnimationEvent->animationSpeed, cameraAnimationEvent->syncWithCharacterAnimation);
	}
}

void AbstractCharacterState::InitializeQTE(const QTE* qte, const int32_t animationEventIndex)
{
	//アクティブ状態にする
	processedQTEDatas_[animationEventIndex].isActive = true;
	//アクションが実行可能な状態でなければQTEを設定しない
	if (!character_->GetActionStateCondition(qte->qteType)) return;
	//QTEの名前を設定
	processedQTEDatas_[animationEventIndex].qteActionName = qte->qteType;
	//QTEの受付時間を設定
	processedQTEDatas_[animationEventIndex].duration = qte->requiredTime;
	//QTEの経過時間を設定
	processedQTEDatas_[animationEventIndex].elapsedTime = qte->requiredTime;
	//アクション開始時にトリガーする状態の場合はQTEを発動
	if (qte->trigger == EventTrigger::kActionStart)
	{
		StartQTE(qte, animationEventIndex);
	}
}

void AbstractCharacterState::InitializeCancelEvent(const CancelEvent* cancelEvent, const int32_t animationEventIndex)
{
	//アクティブ状態にする
	processedCancelDatas_[animationEventIndex].isActive = true;
	//キャンセルアクションの名前を設定
	processedCancelDatas_[animationEventIndex].cancelActionName = cancelEvent->cancelType;
	//先行入力がないかどうかを調べる
	processedCancelDatas_[animationEventIndex].isCanceled = CheckAndTransitionBufferedAction(processedCancelDatas_[animationEventIndex].cancelActionName);
}

void AbstractCharacterState::InitializeBufferedActionEvent(const int32_t animationEventIndex)
{
	//アクティブ状態にする
	processedBufferedActionDatas_[animationEventIndex].isActive = true;
}

void AbstractCharacterState::UpdateAnimationSpeed(const float currentAnimationTime, AnimatorComponent* animator)
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

void AbstractCharacterState::CheckPerfectDodgeWindow(const std::shared_ptr<AnimationEvent>& animationEvent, const float currentAnimationTime)
{
	//既に回避ウィンドウが有効の場合は処理を飛ばす
	if (character_->GetIsVulnerableToPerfectDodge()) return;

	//攻撃開始時間を基準に回避ウィンドウを設定する
	bool isInDodgeWindow = animationEvent->startEventTime - kPerfectDodgeWindowTime <= currentAnimationTime && animationEvent->startEventTime >= currentAnimationTime;
	character_->SetIsVulnerableToPerfectDodge(isInDodgeWindow);
}

const float AbstractCharacterState::GetEasingParameter(const AnimationEvent* animationEvent, const EasingType easingType, const float animationTime) const
{
	//イージング係数を計算
	float easingParameter = (animationTime - animationEvent->startEventTime) / (animationEvent->endEventTime - animationEvent->startEventTime);
	easingParameter = std::clamp(easingParameter, 0.0f, 1.0f);

	//イージングの種類に応じた処理
	switch (easingType)
	{
	case EasingType::kEaseIn:
		return Mathf::EaseInSine(easingParameter);
	case EasingType::kEaseOut:
		return Mathf::EaseOutSine(easingParameter);
	case EasingType::kEaseInOut:
		return Mathf::EaseInOutSine(easingParameter);
	default:
		return easingParameter;
	}
}

void AbstractCharacterState::ProcessAnimationEvents(const float currentAnimationTime)
{
	//アニメーションイベントのインデックス
	int32_t animationEventIndex[static_cast<int>(EventType::kMaxEvent)]{};

	//全てのアニメーションイベントを処理
	for (const std::shared_ptr<AnimationEvent>& animationEvent : animationController_->animationEvents)
	{
		//攻撃イベントの場合
		if (animationEvent->eventType == EventType::kAttack)
		{
			//ジャスト回避を受け付けるかを確認
			CheckPerfectDodgeWindow(animationEvent, currentAnimationTime);
		}

		//現在のアニメーション時間にイベントが発生しているか確認
		if (animationEvent->startEventTime <= currentAnimationTime && animationEvent->endEventTime >= currentAnimationTime)
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

void AbstractCharacterState::ProcessAnimationEvent(const AnimationEvent* animationEvent, const float animationTime, const int32_t animationEventIndex)
{
	//アニメーションイベントの種類に応じて処理を分岐
	switch (animationEvent->eventType)
	{
	case EventType::kMovement:
		ProcessMovementEvent(static_cast<const MovementEvent*>(animationEvent), animationTime, animationEventIndex);
		break;
	case EventType::kRotation:
		ProcessRotationEvent(static_cast<const RotationEvent*>(animationEvent), animationTime, animationEventIndex);
		break;
	case EventType::kAttack:
		ProcessAttackEvent(static_cast<const AttackEvent*>(animationEvent), animationEventIndex);
		break;
	case EventType::kEffect:
		ProcessEffectEvent(static_cast<const EffectEvent*>(animationEvent), animationEventIndex);
		break;
	case EventType::kCameraAnimation:
		ProcessCameraAnimationEvent(static_cast<const CameraAnimationEvent*>(animationEvent), animationEventIndex);
		break;
	case EventType::kQTE:
		ProcessQTE(static_cast<const QTE*>(animationEvent), animationEventIndex);
		break;
	case EventType::kCancel:
		ProcessCancelEvent(static_cast<const CancelEvent*>(animationEvent), animationEventIndex);
		break;
	case EventType::kBufferedAction:
		ProcessBufferedActionEvent(static_cast<const BufferedActionEvent*>(animationEvent), animationEventIndex);
		break;
	}
}

void AbstractCharacterState::ProcessMovementEvent(const MovementEvent* movementEvent, const float animationTime, const int32_t animationEventIndex)
{
	//速度による移動イベントの処理
	if (movementEvent->movementType == MovementType::kVelocity)
	{
		ProcessVelocityMovementEvent(static_cast<const VelocityMovementEvent*>(movementEvent), animationEventIndex);
	}
	//イージングによる移動イベントの処理
	else if (movementEvent->movementType == MovementType::kEasing)
	{
		ProcessEasingMovementEvent(static_cast<const EasingMovementEvent*>(movementEvent), animationTime, animationEventIndex);
	}
}

void AbstractCharacterState::ProcessVelocityMovementEvent(const VelocityMovementEvent* velocityMovementEvent, const int32_t animationEventIndex)
{
	//速度移動イベントがまだアクティブでない場合の初期化処理
	if (!processedVelocityDatas_[animationEventIndex].isActive)
	{
		//速度移動イベントの初期化
		InitializeVelocityMovement(velocityMovementEvent, animationEventIndex);
	}

	//接近補正が有効で、敵が接近した場合
	if (velocityMovementEvent->isProximityStopEnabled && ApplyProximityCorrection())
	{
		//速度をゼロに設定
		processedVelocityDatas_[animationEventIndex].velocity = { 0.0f, 0.0f, 0.0f };
	}

	//キャラクターを移動させる
	character_->Move(processedVelocityDatas_[animationEventIndex].velocity);
}

void AbstractCharacterState::ProcessEasingMovementEvent(const EasingMovementEvent* easingMovementEvent, const float animationTime, const int32_t animationEventIndex)
{
	//イージング移動イベントがまだアクティブでない場合の初期化処理
	if (!processedEasingDatas_[animationEventIndex].isActive)
	{
		//イージング移動イベントの初期化
		InitializeEasingMovementEvent(easingMovementEvent, animationEventIndex);
	}

	//接近補正が有効で、敵が接近した場合
	if (easingMovementEvent->isProximityStopEnabled && ApplyProximityCorrection())
	{
		//現在の位置をイージングの開始位置、ターゲット位置として設定
		processedEasingDatas_[animationEventIndex].startPosition = character_->GetPosition();
		processedEasingDatas_[animationEventIndex].targetPosition = character_->GetPosition();
	}

	//イージング係数を計算
	float easingParameter = GetEasingParameter(easingMovementEvent, easingMovementEvent->easingType, animationTime);

	//キャラクターの位置を更新
	character_->SetPosition(Mathf::Lerp(processedEasingDatas_[animationEventIndex].startPosition, processedEasingDatas_[animationEventIndex].targetPosition, easingParameter));
}

void AbstractCharacterState::ProcessRotationEvent(const RotationEvent* rotationEvent, const float animationTime, const int32_t animationEventIndex)
{
	//回転イベントがまだアクティブでない場合の初期化処理
	if (!processedRotationDatas_[animationEventIndex].isActive)
	{
		InitializeRotationEvent(animationEventIndex);
	}

	//イージング係数を計算
	float easingParameter = GetEasingParameter(rotationEvent, rotationEvent->easingType, animationTime);

	//累計のイージングされた回転量を計算
	float totalEasedRotation = rotationEvent->rotationAngle * easingParameter;

	//フレームごとの回転量を計算
	float frameRotation = totalEasedRotation - processedRotationDatas_[animationEventIndex].preEasedRotation;

	//累積回転量を保存
	processedRotationDatas_[animationEventIndex].preEasedRotation = totalEasedRotation;

	//新しいクォータニオンを作成
	Quaternion newQuaternion = Mathf::MakeRotateAxisAngleQuaternion(Mathf::Normalize(rotationEvent->rotationAxis), frameRotation);

	//現在のクォータニオンに新しい回転を適用
	character_->SetDestinationQuaternion(Mathf::Normalize(character_->GetDestinationQuaternion() * newQuaternion));
}

void AbstractCharacterState::ProcessAttackEvent(const AttackEvent* attackEvent, const int32_t animationEventIndex)
{
	//攻撃イベントがまだアクティブでない場合の初期化処理
	if (!processedAttackDatas_[animationEventIndex].isActive)
	{
		InitializeAttackEvent(attackEvent, animationEventIndex);
	}

	//現在のヒットカウントが最大ヒットカウントに達していない場合
	if (processedAttackDatas_[animationEventIndex].currentHitCount < attackEvent->attackParameters.maxHitCount)
	{
		//武器を取得
		Weapon* weapon = character_->GetWeapon();

		//攻撃タイマーを進める
		processedAttackDatas_[animationEventIndex].elapsedAttackTime += GameTimer::GetDeltaTime() * character_->GetTimeScale();

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

void AbstractCharacterState::ProcessEffectEvent(const EffectEvent* effectEvent, const int32_t animationEventIndex)
{
	//エフェクトイベントがまだアクティブでない場合の初期化処理
	if (!processedEffectDatas_[animationEventIndex].isActive)
	{
		InitializeEffectEvent(effectEvent, animationEventIndex);
	}

	//エミッターの座標を更新
	UpdateEmitterPosition(effectEvent, animationEventIndex);

	//攻撃が敵に当たったらエフェクトを適用する
	if (ShouldTriggerEvent(effectEvent->trigger))
	{ 
		ApplyEffect(effectEvent, processedEffectDatas_[animationEventIndex].emitterPosition);
	}
}

void AbstractCharacterState::UpdateEmitterPosition(const EffectEvent* effectEvent, const int32_t animationEventIndex)
{
	//攻撃ヒット時の場合
	if (effectEvent->trigger == EventTrigger::kImpact)
	{
		//武器を取得
		Weapon* weapon = character_->GetWeapon();

		//武器のトランスフォームを取得
		TransformComponent* transform = weapon->GetComponent<TransformComponent>();

		//エミッターの座標を更新
		processedEffectDatas_[animationEventIndex].emitterPosition = transform->GetWorldPosition() + Mathf::TransformNormal(weapon->GetFrontOffset(), transform->worldTransform_.matWorld_);
	}
	else
	{
		//キャラクターのトランスフォームからエミッター位置を取得
		processedEffectDatas_[animationEventIndex].emitterPosition = character_->GetComponent<TransformComponent>()->GetWorldPosition() + Mathf::RotateVector(character_->GetOriginOffset(), character_->GetQuaternion());
	}
}

void AbstractCharacterState::ApplyEffect(const EffectEvent* effectEvent, const Vector3& emitterPosition)
{
	//ヒットストップ開始
	character_->GetHitStop()->Start(effectEvent->hitStopDuration);
	//カメラシェイク開始
	character_->GetCameraController()->StartCameraShake(effectEvent->cameraShakeIntensity, effectEvent->cameraShakeDuration);
	//パーティクルの生成
	character_->GetEditorManager()->GetParticleEffectEditor()->CreateParticles(effectEvent->particleEffectName, emitterPosition, character_->GetQuaternion());
	//ポストエフェクトを適用
	switch (effectEvent->postEffectType)
	{
	case PostEffectType::kRadialBlur: 
		PostEffects::GetInstance()->GetRadialBlur()->SetIsEnable(true);
		break;
	}
	//サウンドエフェクトを再生
	character_->PlaySoundEffect(effectEvent->soundEffectType);
}

void AbstractCharacterState::ProcessCameraAnimationEvent(const CameraAnimationEvent* cameraAnimationEvent, const int32_t animationEventIndex)
{
	//カメラアニメーションイベントがまだアクティブでない場合の初期化処理
	if (!processedCameraAnimationDatas_[animationEventIndex].isActive)
	{
		InitializeCameraAnimationEvent(cameraAnimationEvent, animationEventIndex);
	}

	//トリガー条件が満たされた場合アニメーションを再生する
	if (!processedCameraAnimationDatas_[animationEventIndex].isAnimationStarted && ShouldTriggerEvent(cameraAnimationEvent->trigger))
	{
		processedCameraAnimationDatas_[animationEventIndex].isAnimationStarted = true;
		character_->GetCameraController()->PlayAnimation(cameraAnimationEvent->animationName, cameraAnimationEvent->animationSpeed, cameraAnimationEvent->syncWithCharacterAnimation);
	}
}

void AbstractCharacterState::ProcessQTE(const QTE* qte, const int32_t animationEventIndex)
{
	//QTEがまだアクティブ状態でない場合の初期化処理
	if (!processedQTEDatas_[animationEventIndex].isActive)
	{
		InitializeQTE(qte, animationEventIndex);
	}

	//QTEを開始可能な条件を満たしていた場合はQTEを開始する
	if (!processedQTEDatas_[animationEventIndex].isQTEActive && !processedQTEDatas_[animationEventIndex].isQTECompleted && !processedQTEDatas_[animationEventIndex].isSuccess && ShouldTriggerEvent(qte->trigger))
	{
		StartQTE(qte, animationEventIndex);
	}

	//QTEが進行中の場合は進捗を更新
	if (processedQTEDatas_[animationEventIndex].isQTEActive)
	{
		UpdateQTEProgress(animationEventIndex);
	}
}

void AbstractCharacterState::StartQTE(const QTE* qte, const int32_t animationEventIndex)
{
	//QTE受付開始
	processedQTEDatas_[animationEventIndex].isQTEActive = true;
	//ヒットストップを無効化する
	character_->GetHitStop()->Stop();
	//ポストエフェクトを有効化
	EnableQTEPostEffects(true);
	//タイムスケールを設定
	GameTimer::SetTimeScale(qte->timeScale);
}

void AbstractCharacterState::UpdateQTEProgress(const int32_t animationEventIndex)
{
	//タイマーを進行
	const float kDeltaTime = 1.0f / 60.0f;
	processedQTEDatas_[animationEventIndex].elapsedTime -= kDeltaTime;

	//QTEの名前が設定されているかつキャラクターが指定のQTEアクションを実行した場合
	if (!processedQTEDatas_[animationEventIndex].qteActionName.empty() && character_->GetActionTriggerCondition(processedQTEDatas_[animationEventIndex].qteActionName))
	{
		//QTE終了処理
		CompleteQTE(processedQTEDatas_[animationEventIndex], true);
		//状態遷移
		character_->ChangeState(processedQTEDatas_[animationEventIndex].qteActionName);
	}
	//タイマーが上限を超えていた場合、またはゲームが終了していた場合
	else if (processedQTEDatas_[animationEventIndex].elapsedTime <= 0.0f || character_->GetIsGameFinished())
	{
		//QTE終了処理
		CompleteQTE(processedQTEDatas_[animationEventIndex], false);
	}
}

void AbstractCharacterState::CompleteQTE(ProcessedQTEData& qteData, const bool isSuccess)
{
	//QTE受付終了、成功かどうかのフラグ設定
	qteData.isQTEActive = false;
	qteData.isSuccess = isSuccess;
	qteData.isQTECompleted = !isSuccess;

	//QTE成功時に全てのQTEの受付終了
	if (isSuccess)
	{
		for (auto& data : processedQTEDatas_)
		{
			data.isQTEActive = false;
		}
	}

	//全てのQTEが非アクティブの場合
	if (std::none_of(processedQTEDatas_.begin(), processedQTEDatas_.end(), [](const auto& data) { return data.isQTEActive; }))
	{
		//タイムスケールリセット、ポストエフェクト無効化
		GameTimer::SetTimeScale(1.0f);
		EnableQTEPostEffects(false);
	}
}

void AbstractCharacterState::EnableQTEPostEffects(const bool isEnable)
{
	//DOFを無効にする
	PostEffects::GetInstance()->GetDepthOfField()->SetIsEnable(isEnable);
	//Vignetteを無効にする
	PostEffects::GetInstance()->GetVignette()->SetIsEnable(isEnable);
}

void AbstractCharacterState::ProcessCancelEvent(const CancelEvent* cancelEvent, const int32_t animationEventIndex)
{
	//キャンセルイベントがまだアクティブでない場合の初期化処理
	if (!processedCancelDatas_[animationEventIndex].isActive)
	{
		InitializeCancelEvent(cancelEvent, animationEventIndex);
	}

	//キャンセルアクションを処理
	HandleCancelAction(cancelEvent, animationEventIndex);
}

void AbstractCharacterState::HandleCancelAction(const CancelEvent* cancelEvent, const int32_t animationEventIndex)
{
	//キャンセルアクションのボタンが押されていたら遷移
	if (character_->IsActionExecutable(cancelEvent->cancelType))
	{
		//キャンセルの条件が設定されていない場合
		if (cancelEvent->cancelType == "None")
		{
			//フラグを立てる
			processedCancelDatas_[animationEventIndex].isCanceled = true;
			//デフォルトの状態遷移を行う
			HandleStateTransition();
		}
		//キャンセルの条件が設定されている場合はその種類に基づいて状態遷移を行う
		else
		{
			processedCancelDatas_[animationEventIndex].isCanceled = character_->ChangeState(cancelEvent->cancelType);
		}
	}
}

void AbstractCharacterState::ProcessBufferedActionEvent(const BufferedActionEvent* bufferedActionEvent, const int32_t animationEventIndex)
{
	//先行入力イベントがまだアクティブでない場合の初期化処理
	if (!processedBufferedActionDatas_[animationEventIndex].isActive)
	{
		InitializeBufferedActionEvent(animationEventIndex);
	}

	//先行入力アクションを処理
	HandleBufferedAction(bufferedActionEvent, animationEventIndex);
}

void AbstractCharacterState::HandleBufferedAction(const BufferedActionEvent* bufferedActionEvent, const int32_t animationEventIndex)
{
	//先行入力が一度も設定されていない場合かつ、キャンセルアクションのボタンが押された場合に先行入力を設定する
	if (!processedBufferedActionDatas_[animationEventIndex].isBufferedInputActive && character_->IsActionExecutable(bufferedActionEvent->bufferedActionType))
	{
		processedBufferedActionDatas_[animationEventIndex].bufferedActionName = bufferedActionEvent->bufferedActionType;
		processedBufferedActionDatas_[animationEventIndex].isBufferedInputActive = true;
	}
}

const bool AbstractCharacterState::ApplyProximityCorrection() const
{
	//接近していない場合は補正しない
	if (!IsOpponentInProximity()) return false;

	//ゲームオブジェクトマネージャーを取得
	GameObjectManager* gameObjectManager = GameObjectManager::GetInstance();

	//自分の座標を取得
	Vector3 myPosition = character_->GetJointWorldPosition("mixamorig:Hips");

	//相手の座標を取得
	Vector3 otherPosition = (character_->GetName() == "Player") ? gameObjectManager->GetGameObject<Enemy>("Enemy")->GetJointWorldPosition("mixamorig:Hips") :
		gameObjectManager->GetGameObject<Player>("Player")->GetJointWorldPosition("mixamorig:Hips");

	//自分と相手の差分ベクトルを計算
	Vector3 diff = otherPosition - myPosition;

	//水平方向の距離を求めるのでY軸を0にする
	diff.y = 0.0f;

	//現在の距離と接近距離との差を求める
	float sub = kProximityDistance - Mathf::Length(diff);

	//差分距離に基づいて補正移動ベクトルを計算
	Vector3 correctionVelocity = Mathf::Normalize(diff) * sub;

	//補正移動をキャラクターに適用
	character_->Move(correctionVelocity);

	//接近補正が適用された
	return true;
}

const bool AbstractCharacterState::ShouldTriggerEvent(const EventTrigger trigger) const
{
	//イベントトリガーの種類によって処理を分岐
	switch (trigger)
	{
	case EventTrigger::kImpact:
		//攻撃がヒットした場合にイベントをトリガー
		return character_->GetWeapon()->GetIsHit();
	case EventTrigger::kEnemyJustDodgeWindow:
		//敵がジャスト回避ウィンドウにいるかつジャスト回避されていない場合にイベントをトリガー
		return IsEnemyInDodgeWindow();
	case EventTrigger::kEnemyStunned:
		//敵がスタン状態の場合にイベントをトリガー
		return IsEnemyStunned();
	default:
		//デフォルトではイベントをトリガーしない
		return false;
	}
}

const bool AbstractCharacterState::IsEnemyInDodgeWindow() const
{
	//ゲームオブジェクトマネージャーを取得
	GameObjectManager* gameObjectManager = GameObjectManager::GetInstance();

	//ジャスト回避される状態でなければ処理を飛ばす
	if ((character_->GetName() == "Player") ? !gameObjectManager->GetGameObject<Enemy>("Enemy")->GetIsVulnerableToPerfectDodge() : !gameObjectManager->GetGameObject<Player>("Player")->GetIsVulnerableToPerfectDodge()) return false;

	//敵との距離が遠ければ処理を飛ばす
	if (!IsOpponentInProximity()) return false;

	//ジャスト回避成功
	return true;
}

const bool AbstractCharacterState::IsEnemyStunned() const
{
	//ゲームオブジェクトマネージャーを取得
	GameObjectManager* gameObjectManager = GameObjectManager::GetInstance();
	//スタン状態かどうかを返す
	return (character_->GetName() == "Player") ? gameObjectManager->GetGameObject<Enemy>("Enemy")->GetIsStunTriggered() :
		gameObjectManager->GetGameObject<Player>("Player")->GetIsStunTriggered();
}

void AbstractCharacterState::InitializeProcessedData()
{
	//各イベントタイプに対する処理データを初期化
	processedVelocityDatas_.resize(animationController_->GetAnimationEventCount(EventType::kMovement));
	processedEasingDatas_.resize(animationController_->GetAnimationEventCount(EventType::kMovement));
	processedRotationDatas_.resize(animationController_->GetAnimationEventCount(EventType::kRotation));
	processedAttackDatas_.resize(animationController_->GetAnimationEventCount(EventType::kAttack));
	processedEffectDatas_.resize(animationController_->GetAnimationEventCount(EventType::kEffect));
	processedCameraAnimationDatas_.resize(animationController_->GetAnimationEventCount(EventType::kCameraAnimation));
	processedCancelDatas_.resize(animationController_->GetAnimationEventCount(EventType::kCancel));
	processedBufferedActionDatas_.resize(animationController_->GetAnimationEventCount(EventType::kBufferedAction));
	processedQTEDatas_.resize(animationController_->GetAnimationEventCount(EventType::kQTE));
}

void AbstractCharacterState::ResetProcessedData(const EventType eventType, const int32_t animationEventIndex)
{
	//各イベントタイプに応じて処理データをリセット
	switch (eventType)
	{
	case EventType::kMovement:
		ResetMovementData(animationEventIndex);
		break;
	case EventType::kRotation:
		ResetRotationData(animationEventIndex);
		break;
	case EventType::kAttack:
		ResetAttackData(animationEventIndex);
		break;
	case EventType::kEffect:
		ResetEffectData(animationEventIndex);
		break;
	case EventType::kCameraAnimation:
		ResetCameraAnimationData(animationEventIndex);
		break;
	case EventType::kQTE:
		ResetQTEData(animationEventIndex);
		break;
	case EventType::kCancel:
		ResetCancelData(animationEventIndex);
		break;
	case EventType::kBufferedAction:
		ResetBufferedActionData(animationEventIndex);
		break;
	}
}

void AbstractCharacterState::ResetMovementData(const int32_t animationEventIndex)
{
	//速度移動イベントが存在しているかつアクティブ状態の場合はアクティブフラグをリセット
	if (!processedVelocityDatas_.empty() && processedVelocityDatas_[animationEventIndex].isActive)
	{
		processedVelocityDatas_[animationEventIndex].isActive = false;
	}
	
	//イージング移動イベントが存在しているかつアクティブ状態の場合はアクティブフラグをリセット
	if (!processedEasingDatas_.empty() && processedEasingDatas_[animationEventIndex].isActive)
	{
		processedEasingDatas_[animationEventIndex].isActive = false;
	}
}

void AbstractCharacterState::ResetRotationData(const int32_t animationEventIndex)
{
	//回転イベントが存在しているかつアクティブ状態の場合はアクティブフラグをリセット
	if (processedRotationDatas_[animationEventIndex].isActive)
	{
		processedRotationDatas_[animationEventIndex].isActive = false;
	}
}

void AbstractCharacterState::ResetAttackData(const int32_t animationEventIndex)
{
	//攻撃イベントが存在しているかつアクティブ状態の場合
	if (processedAttackDatas_[animationEventIndex].isActive)
	{
		//アクティブフラグをリセット
		processedAttackDatas_[animationEventIndex].isActive = false;
		//武器を取得
		Weapon* weapon = character_->GetWeapon();
		//軌跡エフェクトを無効化
		weapon->SetIsTrailActive(false);
		//攻撃状態を無効化
		weapon->SetIsAttack(false);
	}
}

void AbstractCharacterState::ResetEffectData(const int32_t animationEventIndex)
{
	//エフェクトイベントが存在しているかつアクティブ状態の場合
	if (processedEffectDatas_[animationEventIndex].isActive)
	{
		//アクティブフラグをリセット
		processedEffectDatas_[animationEventIndex].isActive = false;
		//ポストエフェクトを無効化
		switch (processedEffectDatas_[animationEventIndex].postEffectType)
		{
		case PostEffectType::kRadialBlur:
			PostEffects::GetInstance()->GetRadialBlur()->SetIsEnable(false);
			break;
		}
	}
}

void AbstractCharacterState::ResetCameraAnimationData(const int32_t animationEventIndex)
{
	//カメラアニメーションイベントが存在しているかつアクティブ状態の場合はアクティブフラグをリセット
	if (processedCameraAnimationDatas_[animationEventIndex].isActive)
	{
		processedCameraAnimationDatas_[animationEventIndex].isActive = false;
	}
}

void AbstractCharacterState::ResetQTEData(const int32_t animationEventIndex)
{
	//QTEが存在しているかつアクティブ状態の場合
	if (processedQTEDatas_[animationEventIndex].isActive)
	{
		//アクティブフラグをリセット
		processedQTEDatas_[animationEventIndex].isActive = false;
		processedQTEDatas_[animationEventIndex].isQTEActive = false;

		//全てのQTEが終了していた場合
		if (std::all_of(processedQTEDatas_.begin(), processedQTEDatas_.end(), [](const auto& data) { return !data.isQTEActive; }))
		{
			//タイムスケールをリセット
			GameTimer::SetTimeScale(1.0f);
			//ポストエフェクトをリセット
			EnableQTEPostEffects(false);
		}
	}
}

void AbstractCharacterState::ResetCancelData(const int32_t animationEventIndex)
{
	//キャンセルイベントが存在しているかつアクティブ状態の場合はアクティブフラグをリセット
	if (processedCancelDatas_[animationEventIndex].isActive)
	{
		processedCancelDatas_[animationEventIndex].isActive = false;
	}
}

void AbstractCharacterState::ResetBufferedActionData(const int32_t animationEventIndex)
{
	//先行入力イベントが存在しているかつアクティブ状態の場合はアクティブフラグをリセット
	if (processedBufferedActionDatas_[animationEventIndex].isActive)
	{
		processedBufferedActionDatas_[animationEventIndex].isActive = false;
	}
}