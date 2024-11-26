#include "IPlayerState.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"

Player* IPlayerState::GetPlayer() const
{
	return dynamic_cast<Player*>(GetCharacter());
}

void IPlayerState::InitializeVelocityMovement(const VelocityMovementEvent* velocityMovementEvent, const int32_t animationEventIndex)
{
	//アクティブ状態にする
	processedVelocityDatas_[animationEventIndex].isActive = true;

	//敵に向かって移動する場合
	if(velocityMovementEvent->moveTowardsEnemy && GetPlayer()->GetLockon()->ExistTarget())
	{
		//プレイヤーと敵の方向ベクトルを計算
		Vector3 direction = CalculateDirectionToEnemy();

		//速度を計算
		processedVelocityDatas_[animationEventIndex].velocity = direction * Mathf::Length(velocityMovementEvent->velocity);
	}
	else if (velocityMovementEvent->useStickInput)
	{
		//スティック入力に基づいて移動ベクトルを計算
		processedVelocityDatas_[animationEventIndex].velocity = ProcessStickInputMovement(velocityMovementEvent->velocity, GetPlayer()->GetRootParameters().walkThreshold);
	}
	else
	{
		//固定速度を使用して移動ベクトルを設定
		processedVelocityDatas_[animationEventIndex].velocity = Mathf::RotateVector(velocityMovementEvent->velocity, character_->GetQuaternion());
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
		//プレイヤーと敵の方向ベクトルを計算
		Vector3 direction = CalculateDirectionToEnemy();

		//目標座標を計算
		processedEasingDatas_[animationEventIndex].targetPosition = direction * Mathf::Length(easingMovementEvent->targetPosition);
	}
	else if (easingMovementEvent->useStickInput)
	{
		//スティック入力に基づいて目標座標を計算
		processedEasingDatas_[animationEventIndex].targetPosition = ProcessStickInputMovement(easingMovementEvent->targetPosition, GetPlayer()->GetRootParameters().walkThreshold);
	}
	else
	{
		//固定座標を使用して目標座標を設定
		processedEasingDatas_[animationEventIndex].targetPosition = Mathf::RotateVector(easingMovementEvent->targetPosition, character_->GetQuaternion());
	}

	//開始座標を足す
	processedEasingDatas_[animationEventIndex].targetPosition = processedEasingDatas_[animationEventIndex].startPosition + processedEasingDatas_[animationEventIndex].targetPosition;

	//進行方向に回転するフラグが立っていた場合はキャラクターを回転させる
	if (easingMovementEvent->rotateTowardsMovement)
	{
		Vector3 difference = processedEasingDatas_[animationEventIndex].targetPosition - processedEasingDatas_[animationEventIndex].startPosition;
		character_->Rotate(Mathf::Normalize(Vector3{ difference.x, 0.0f, difference.z }));
	}
}

void IPlayerState::HandleStateTransitionInternal()
{
	//プレイヤーが地面にいなかった場合は落下状態にする
	Vector3 position = character_->GetPosition();
	if (position.y > 0.0f)
	{
		character_->ChangeState("Falling");
	}
	//地面にいる場合は地面に埋まらないように座標を補正して通常状態に戻す
	else
	{
		character_->SetPosition({ position.x, 0.0f, position.z });
		character_->ChangeState("Idle");
	}
}

Vector3 IPlayerState::CalculateDirectionToEnemy() const
{
	//プレイヤーの座標を取得
	Vector3 playerPosition = character_->GetPosition();

	//敵の座標を取得
	Vector3 enemyPosition = GameObjectManager::GetInstance()->GetGameObject<Enemy>("Enemy")->GetPosition();

	//差分ベクトルを計算
	Vector3 difference = enemyPosition - playerPosition;

	//Y成分の差が許容範囲内であれば、速度のY成分を0に設定
	if (std::abs(difference.y) < maxAllowableYDifference)
	{
		difference.y = 0.0f;
	}

	//正規化して返す
	return Mathf::Normalize(difference);
}

Vector3 IPlayerState::ProcessStickInputMovement(const Vector3& velocity, const float walkThreshold) const
{
	//スティックの入力値を取得
	Vector3 inputValue = { input_->GetLeftStickX(), 0.0f, input_->GetLeftStickY() };

	//スティック入力が閾値を超えていた場合
	if (Mathf::Length(inputValue) > walkThreshold)
	{
		//入力値に基づいて移動ベクトルを計算
		Vector3 movementVelocity = Mathf::Normalize(inputValue) * Mathf::Length({ velocity.x, 0.0f, velocity.z });
		movementVelocity = Mathf::RotateVector(movementVelocity, GetPlayer()->GetCameraController()->GetCamera().quaternion_);
		movementVelocity.y = velocity.y;
		return movementVelocity;
	}

	//閾値以下の場合、固定速度での移動ベクトルを設定
	return Mathf::RotateVector(velocity, character_->GetQuaternion());
}
