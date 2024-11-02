#include "IPlayerState.h"
#include "Application/Src/Object/Character/Player/Player.h"

Player* IPlayerState::GetPlayer() const
{
	return dynamic_cast<Player*>(GetCharacter());
}

void IPlayerState::InitializeVelocityMovement(const VelocityMovementEvent* velocityMovementEvent)
{
	//アクティブ状態にする
	processedVelocityData_.isActive = true;

	//スティック入力を使用するかどうかで速度の計算方法を変える
	if (velocityMovementEvent->useStickInput)
	{
		//スティックの入力値を取得
		Vector3 inputValue = { input_->GetLeftStickX(), 0.0f, input_->GetLeftStickY() };

		//スティックの入力に基づいて速度を設定
		processedVelocityData_.velocity = Mathf::Normalize(inputValue) * velocityMovementEvent->velocity;
	}
	else
	{
		//固定速度を使用して移動ベクトルを設定
		processedVelocityData_.velocity = velocityMovementEvent->velocity;
	}

	//キャラクターの向きに基づいて速度ベクトルを回転
	processedVelocityData_.velocity = Mathf::RotateVector(processedVelocityData_.velocity, velocityMovementEvent->useStickInput ? 
		GetPlayer()->GetCamera()->quaternion_ : character_->GetQuaternion());

	//スティックの入力をしている場合は速度のY成分を0にする
	if (velocityMovementEvent->useStickInput)
	{
		processedVelocityData_.velocity.y = 0.0f;
	}
}

void IPlayerState::InitializeEasingMovementEvent(const EasingMovementEvent* easingMovementEvent)
{
	//アクティブ状態にする
	processedEasingData_.isActive = true;

	//現在の位置を開始位置として保存
	processedEasingData_.startPosition = character_->GetPosition();

	//スティック入力を使用するかどうかで目標座標の計算方法を変える
	if (easingMovementEvent->useStickInput)
	{
		//スティックの入力値を取得
		Vector3 inputValue = { input_->GetLeftStickX(), 0.0f, input_->GetLeftStickY() };

		//スティックの入力に基づいて目標座標を計算
		processedEasingData_.targetPosition = inputValue * easingMovementEvent->targetPosition;
	}
	else
	{
		//固定座標を使用して目標座標を設定
		processedEasingData_.targetPosition = easingMovementEvent->targetPosition;
	}

	//y成分の設定
	processedEasingData_.targetPosition.y = easingMovementEvent->targetPosition.y;

	//キャラクターの向きに基づいて速度ベクトルを回転
	processedEasingData_.targetPosition = processedEasingData_.startPosition + Mathf::RotateVector(processedEasingData_.targetPosition, easingMovementEvent->useStickInput ? 
		GetPlayer()->GetCamera()->quaternion_ : character_->GetQuaternion());
}