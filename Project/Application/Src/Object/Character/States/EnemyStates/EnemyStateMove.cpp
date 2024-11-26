#include "EnemyStateMove.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"
#include "Application/Src/Object/Character/Player/Player.h"

void EnemyStateMove::Initialize()
{
	//アニメーションブレンドを有効にする
	character_->GetAnimator()->SetIsBlending(true);

	//アニメーションブレンドの時間を設定
	character_->GetAnimator()->SetBlendDuration(0.2f);
}

void EnemyStateMove::Update()
{
	//デバッグのフラグが立っている、またはゲームが終了している場合は処理を飛ばす
	if (GetEnemy()->GetIsDebug() || character_->GetIsGameFinished()) return;

	//プレイヤーの方向に向かせる
	GetEnemy()->LookAtPlayer();

	//移動ベクトルを回転させる
	RotateVelocity();

	//アニメーションイベントを実行
	UpdateAnimationState();

	//攻撃状態への遷移を試行
	if (TryTransitionToAttackState()) return;

	//回避状態への遷移を試行
	if (TryTransitionToDodgeState()) return;

	//待機タイマーを更新し、必要なら状態遷移
	UpdateMovementWaitTimer();
}

void EnemyStateMove::OnCollision(GameObject* other)
{
	//衝突処理
	character_->ProcessCollisionImpact(other, true);
}

void EnemyStateMove::InitializeRandomWaitTimeBeforeMovement(const float minWaitTime, const float maxWaitTime)
{
	//待機時間をランダムに設定
	currentWaitTimeBeforeMovement_ = RandomGenerator::GetRandomFloat(minWaitTime, maxWaitTime);
}

void EnemyStateMove::RotateVelocity()
{
	//移動イベントが存在しているかつアクティブ状態の場合
	if (!processedVelocityDatas_.empty() && processedVelocityDatas_[0].isActive)
	{
		//移動ベクトルを回転させる
		processedVelocityDatas_[0].velocity = Mathf::RotateVector(processedVelocityDatas_[0].velocity, character_->GetQuaternion());
	}
}

bool EnemyStateMove::TryTransitionToAttackState() 
{
	//攻撃可能な状態なら攻撃状態に遷移する
	if (GetEnemy()->GetActionFlag(Enemy::ActionFlag::kCanAttack))
	{
		return DecideAttackAction();
	}
	return false;
}

bool EnemyStateMove::DecideAttackAction()
{
	//遷移可能なアクション一覧
	const std::vector<std::string> actions = { "Approach", "Attack" };

	//アクションリストをループし、遷移条件をチェック
	for (const auto& action : actions)
	{
		//遷移条件が満たされていた場合
		if (character_->GetActionCondition(action))
		{
			//対応する状態に遷移
			character_->ChangeState(action);
			return true;
		}
	}

	//遷移失敗
	return false;
}

bool EnemyStateMove::TryTransitionToDodgeState()
{
	//プレイヤーが攻撃しているかつ回避可能な状態の場合
	if (GetEnemy()->GetActionFlag(Enemy::ActionFlag::kCanDodge))
	{
		//回避条件が満たされていない場合は処理を飛ばす
		if (!character_->GetActionCondition("Dodge")) return false;

		//プレイヤーが攻撃中でなければ処理を飛ばす
		if (!GameObjectManager::GetInstance()->GetGameObject<Player>("Player")->GetActionFlag(Player::ActionFlag::kIsAttacking)) return false;

		//回避状態に遷移
		character_->ChangeState("Dodge");
	}
	return false;
}

void EnemyStateMove::UpdateMovementWaitTimer()
{
	//待機時間のタイマーを更新
	waitTimeBeforeMovementTimer_ += GameTimer::GetDeltaTime() * character_->GetTimeScale();

	//待機時間のタイマーが一定時間を超えていたら状態を遷移
	if (waitTimeBeforeMovementTimer_ > currentWaitTimeBeforeMovement_)
	{
		HandleStateTransition();
	}
}