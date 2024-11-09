#include "IPlayerState.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Player/Player.h"

Player* IPlayerState::GetPlayer() const
{
	return dynamic_cast<Player*>(GetCharacter());
}

void IPlayerState::HandleStateTransition()
{
	//先行入力があった場合はその状態に遷移して処理を飛ばす
	if (CheckAndTransitionBufferedAction())
	{
		return;
	}

	//座標を取得
	Vector3 position = character_->GetPosition();

	//プレイヤーが地面にいなかった場合は落下状態にする
	if (position.y > 0.0f)
	{
		character_->ChangeState("Falling");
	}
	//地面にいる場合は地面に埋まらないように座標を補正して通常状態に戻す
	else
	{
		character_->SetPosition({ position.x, 0.0f, position.z });
		character_->ChangeState("Move");
	}
}

void IPlayerState::InitializeVelocityMovement(const VelocityMovementEvent* velocityMovementEvent, const int32_t animationEventIndex)
{
	//アクティブ状態にする
	processedVelocityDatas_[animationEventIndex].isActive = true;

	//敵に向かって移動する場合
	if(velocityMovementEvent->moveTowardsEnemy && GetPlayer()->GetLockon()->ExistTarget())
	{
		//プレイヤーの座標を取得
		Vector3 playerPosition = character_->GetJointWorldPosition("mixamorig:Hips");

		//敵の座標を取得
		Vector3 enemyPosition = GameObjectManager::GetInstance()->GetGameObject<Enemy>("Enemy")->GetJointWorldPosition("mixamorig:Hips");

		//差分ベクトルを計算
		Vector3 difference = enemyPosition - playerPosition;

		//速度を計算
		processedVelocityDatas_[animationEventIndex].velocity = Mathf::Normalize(difference) * velocityMovementEvent->moveSpeed;

		//進行方向に回転させる
		character_->Rotate(Mathf::Normalize(Vector3{ processedVelocityDatas_[animationEventIndex].velocity.x, 0.0f, processedVelocityDatas_[animationEventIndex].velocity.z }));
	
		//Y軸の差分が許容誤差以内だった場合Y軸の速度を0にする
		if (std::abs(difference.y) < GetPlayer()->GetVerticalAlignmentTolerance())
		{
			processedVelocityDatas_[animationEventIndex].velocity.y = 0.0f;
		}
	}
	else if (velocityMovementEvent->useStickInput)
	{
		//スティックの入力値を取得
		Vector3 inputValue = { input_->GetLeftStickX(), 0.0f, input_->GetLeftStickY() };

		//スティック入力が閾値を超えていた場合
		if (Mathf::Length(inputValue) > GetPlayer()->GetRootParameters().walkThreshold)
		{
			processedVelocityDatas_[animationEventIndex].velocity = Mathf::Normalize(inputValue) * velocityMovementEvent->moveSpeed;
			processedVelocityDatas_[animationEventIndex].velocity = Mathf::RotateVector(processedVelocityDatas_[animationEventIndex].velocity, GetPlayer()->GetCamera()->quaternion_);
			processedVelocityDatas_[animationEventIndex].velocity.y = 0.0f;
		}
		else
		{
			//固定速度を使用して移動ベクトルを設定
			processedVelocityDatas_[animationEventIndex].velocity = Mathf::RotateVector({ 0.0f, 0.0f, velocityMovementEvent->moveSpeed }, character_->GetQuaternion());
			processedVelocityDatas_[animationEventIndex].velocity.y = 0.0f;
		}
	}
	else
	{
		//固定速度を使用して移動ベクトルを設定
		processedVelocityDatas_[animationEventIndex].velocity = Mathf::RotateVector({ 0.0f, 0.0f, velocityMovementEvent->moveSpeed }, character_->GetQuaternion());
		processedVelocityDatas_[animationEventIndex].velocity.y = 0.0f;
	}

	//進行方向に回転するフラグが立っていた場合はキャラクターを回転させる
	if (velocityMovementEvent->rotateTowardsMovement)
	{
		character_->Rotate(Mathf::Normalize(Vector3{ processedVelocityDatas_[animationEventIndex].velocity.x, 0.0f, processedVelocityDatas_[animationEventIndex].velocity.z }));
	}
}

void IPlayerState::InitializeEasingMovementEvent(const EasingMovementEvent* easingMovementEvent, const int32_t animationEventIndex)
{
	//アクティブ状態にする
	processedEasingDatas_[animationEventIndex].isActive = true;

	//現在の位置を開始位置として保存
	processedEasingDatas_[animationEventIndex].startPosition = character_->GetPosition();

	//敵に向かって移動する場合
	if (easingMovementEvent->moveTowardsEnemy && GetPlayer()->GetLockon()->ExistTarget())
	{
		//プレイヤーの座標を取得
		Vector3 playerPosition = character_->GetJointWorldPosition("mixamorig:Hips");

		//敵の座標を取得
		Vector3 enemyPosition = GameObjectManager::GetInstance()->GetGameObject<Enemy>("Enemy")->GetJointWorldPosition("mixamorig:Hips");

		//目標座標を計算
		processedEasingDatas_[animationEventIndex].targetPosition = Mathf::Normalize(enemyPosition - playerPosition) * easingMovementEvent->targetPosition;
	}
	else if (easingMovementEvent->useStickInput)
	{
		//スティックの入力値を取得
		Vector3 inputValue = { input_->GetLeftStickX(), 0.0f, input_->GetLeftStickY() };

		//スティック入力が閾値を超えていた場合
		if (Mathf::Length(inputValue) > GetPlayer()->GetRootParameters().walkThreshold)
		{
			processedEasingDatas_[animationEventIndex].targetPosition = Mathf::Normalize(inputValue) * easingMovementEvent->targetPosition;
			processedEasingDatas_[animationEventIndex].targetPosition = Mathf::RotateVector(processedEasingDatas_[animationEventIndex].targetPosition, GetPlayer()->GetCamera()->quaternion_);
			processedEasingDatas_[animationEventIndex].targetPosition.y = 0.0f;
		}
		else
		{
			//固定座標を使用して目標座標を設定
			processedEasingDatas_[animationEventIndex].targetPosition = Mathf::RotateVector(easingMovementEvent->targetPosition, character_->GetQuaternion());
		}
	}
	else
	{
		//固定座標を使用して目標座標を設定
		processedEasingDatas_[animationEventIndex].targetPosition = Mathf::RotateVector(easingMovementEvent->targetPosition, character_->GetQuaternion());
	}

	//開始座標を足す
	processedEasingDatas_[animationEventIndex].targetPosition += processedEasingDatas_[animationEventIndex].startPosition;

	//進行方向に回転するフラグが立っていた場合はキャラクターを回転させる
	if (easingMovementEvent->rotateTowardsMovement)
	{
		Vector3 difference = processedEasingDatas_[animationEventIndex].targetPosition - processedEasingDatas_[animationEventIndex].startPosition;
		character_->Rotate(Mathf::Normalize(Vector3{ difference.x, 0.0f, difference.z }));
	}
}

void IPlayerState::HandleCancelAction(const CancelEvent* cancelEvent, const int32_t animationEventIndex)
{
	//キャンセルアクションのボタンが押されていたら遷移
	if (GetPlayer()->IsButtonTriggered(cancelEvent->cancelType))
	{
		//キャンセルされたフラグを立てる
		processedCancelDatas_[animationEventIndex].isCanceled = true;

		//新しい状態に遷移
		character_->ChangeState(cancelEvent->cancelType);
	}
}

void IPlayerState::HandleBufferedAction(const BufferedActionEvent* bufferedActionEvent, const int32_t animationEventIndex)
{
	//キャンセルアクションのボタンが押されていたら遷移
	if (GetPlayer()->IsButtonTriggered(bufferedActionEvent->bufferedActionType))
	{
		processedBufferedActionDatas_[animationEventIndex].bufferedActionName = bufferedActionEvent->bufferedActionType;
		processedBufferedActionDatas_[animationEventIndex].isBufferedInputActive = true;
	}
}