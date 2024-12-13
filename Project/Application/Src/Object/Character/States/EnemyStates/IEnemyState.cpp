/**
 * @file EnemyStateStun.cpp
 * @brief 敵の状態の基底クラスを管理するファイル
 * @author 青木智滉
 * @date
 */

#include "IEnemyState.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"
#include "Application/Src/Object/Character/Player/Player.h"

Enemy* IEnemyState::GetEnemy() const
{
	return dynamic_cast<Enemy*>(GetCharacter());
}

void IEnemyState::InitializeVelocityMovement(const VelocityMovementEvent* velocityMovementEvent, const int32_t animationEventIndex)
{
	//アクティブ状態にする
	processedEasingDatas_[animationEventIndex].isActive = true;

	//敵に向かって移動する場合
	if (velocityMovementEvent->moveTowardsEnemy)
	{
		//プレイヤーとの方向を計算
		Vector3 direction = CalculateDirectionToPlayer();

		//速度を計算
		processedVelocityDatas_[animationEventIndex].velocity = direction * Mathf::Length(velocityMovementEvent->velocity);
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

void IEnemyState::InitializeEasingMovementEvent(const EasingMovementEvent* easingMovementEvent, const int32_t animationEventIndex)
{
	//アクティブ状態にする
	processedEasingDatas_[animationEventIndex].isActive = true;

	//現在の位置を開始位置として保存
	processedEasingDatas_[animationEventIndex].startPosition = character_->GetPosition();

	//敵に向かって移動する場合
	if (easingMovementEvent->moveTowardsEnemy)
	{
		//プレイヤーとの方向を計算
		Vector3 direction = CalculateDirectionToPlayer();

		//目標座標を計算
		processedEasingDatas_[animationEventIndex].targetPosition = direction * Mathf::Length(easingMovementEvent->targetPosition);
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

void IEnemyState::HandleStateTransitionInternal()
{
	//遷移可能なアクション一覧
	const std::vector<std::string> actions = { "Idle", "MoveTowardPlayer", "MoveAwayFromPlayer", "MoveSideToPlayer" };

	//アクションリストをループし、遷移条件をチェック
	for (const auto& action : actions)
	{
		//遷移条件が満たされていた場合
		if (character_->IsActionExecutable(action))
		{
			//対応する状態に遷移
			character_->ChangeState(action);
			return;
		}
	}
}

Vector3 IEnemyState::CalculateDirectionToPlayer() const
{
	//敵の座標を取得
	Vector3 enemyPosition = character_->GetPosition();

	//プレイヤーの座標を取得
	Vector3 playerPosition = GameObjectManager::GetInstance()->GetGameObject<Player>("Player")->GetPosition();

	//差分ベクトルを計算
	Vector3 difference = playerPosition - enemyPosition;

	//Y成分を0に設定
	difference.y = 0.0f;

	//正規化して返す
	return Mathf::Normalize(difference);
}