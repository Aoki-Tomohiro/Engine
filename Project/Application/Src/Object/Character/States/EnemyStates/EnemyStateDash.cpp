#include "EnemyStateDash.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"
#include "Application/Src/Object/Character/States/EnemyStates/EnemyStateRoot.h"
#include "Application/Src/Object/Character/States/EnemyStates/EnemyStateTackle.h"
#include "Application/Src/Object/Character/States/EnemyStates/EnemyStateJumpAttack.h"
#include "Application/Src/Object/Character/States/EnemyStates/EnemyStateComboAttack.h"
#include "Application/Src/Object/Character/Player/Player.h"

void EnemyStateDash::Initialize()
{
	//ダッシュアニメーションを再生
	enemy_->GetAnimator()->PlayAnimation("Dash", 1.0f, false);

	//プレイヤーの方向に回転
	RotateTowardsPlayer();

	//開始座標と終了座標を設定
	startPosition_ = enemy_->GetPosition();
	endPosition_ = CalculateEndPosition();
}

void EnemyStateDash::Update()
{
	//ダッシュが終了してない場合移動させる
	if (!isDashFinished_)
	{
		UpdateDashMovement();
	}

	//プレイヤーに近づいたら攻撃を始める
	if (ShouldStartCloseRangeAttack())
	{
		HandleCloseRangeAttack();
		return;
	}

	//ダッシュが終了しているかどうかを判定し、次の状態に遷移
	if (isDashFinished_ && enemy_->GetAnimator()->GetIsBlendingCompleted())
	{
		TransitionToRandomState();
	}

	//ダッシュが終了していない状態の場合
	if (!isDashFinished_)
	{
		//アニメーションが終了したらダッシュ行動を終了
		if (enemy_->GetAnimator()->GetIsAnimationFinished())
		{
			FinishDash();
		}
	}
}

void EnemyStateDash::UpdateDashMovement()
{
	//現在のアニメーションの時間を取得し、イージング係数を計算
	float easingParameter = GetEasingParameter();

	//目標座標までイージング
	Vector3 easedPosition = startPosition_ + (endPosition_ - startPosition_) * Mathf::EaseOutCubic(easingParameter);

	//敵の座標を設定
	enemy_->SetPosition(easedPosition);
}

void EnemyStateDash::FinishDash()
{
	//ダッシュの終了をフラグを立てて、次のアニメーションを再生
	isDashFinished_ = true;
	enemy_->GetAnimator()->SetBlendDuration(0.4f);
	enemy_->GetAnimator()->PlayAnimation("Idle", 1.0f, true);
}

void EnemyStateDash::OnCollision(GameObject* other)
{
	//衝突処理
	enemy_->ProcessCollisionImpact(other, false);
}

Vector3 EnemyStateDash::CalculateDirectionToPlayer() const
{
	//プレイヤーの位置と敵の位置から方向ベクトルを計算し、Y軸方向を無視
	Vector3 playerPosition = GameObjectManager::GetInstance()->GetGameObject<Player>("Player")->GetJointWorldPosition("mixamorig:Hips");
	Vector3 enemyPosition = enemy_->GetJointWorldPosition("mixamorig:Hips");
	Vector3 directionToPlayer = playerPosition - enemyPosition;
	directionToPlayer.y = 0.0f;
	return directionToPlayer;
}

const bool EnemyStateDash::ShouldStartCloseRangeAttack() const
{
	//プレイヤーへの方向ベクトルを計算し、距離が近いかどうかを判定
	return Mathf::Length(CalculateDirectionToPlayer()) < enemy_->GetRootParameters().stopDistance;
}

void EnemyStateDash::RotateTowardsPlayer()
{
	//プレイヤーへの方向ベクトルを計算し、その方向に敵を回転させる
	Vector3 directionToPlayer = CalculateDirectionToPlayer();
	enemy_->Rotate(Mathf::Normalize(directionToPlayer));
}

void EnemyStateDash::HandleCloseRangeAttack()
{
	//ランダムに攻撃を選択し、近接攻撃状態に遷移
	Enemy::CloseRangeAttack nextAction = Enemy::CloseRangeAttack::kMaxCloseRangeAttacks;
	do
	{
		nextAction = static_cast<Enemy::CloseRangeAttack>(RandomGenerator::GetRandomInt(0, static_cast<int>(Enemy::CloseRangeAttack::kMaxCloseRangeAttacks)));
	} while (nextAction == enemy_->GetPreviousCloseRangeAttack());
	PerformCloseRangeAttack(static_cast<int>(nextAction));
}

void EnemyStateDash::PerformCloseRangeAttack(const int attack)
{
	//現在の攻撃を設定し、攻撃タイプに応じた処理を実行
	Enemy::CloseRangeAttack nextAttack = static_cast<Enemy::CloseRangeAttack>(attack);
	enemy_->SetPreviousCloseRangeAttack(nextAttack);

	//攻撃タイプに応じた処理を実行
	switch (nextAttack)
	{
	case Enemy::CloseRangeAttack::kTackle:
		//タックル攻撃の処理
		enemy_->ChangeState(new EnemyStateTackle());
		break;
	case Enemy::CloseRangeAttack::kJumpAttack:
		//ジャンプ攻撃の処理
		enemy_->ChangeState(new EnemyStateJumpAttack());
		break;
	case Enemy::CloseRangeAttack::kComboAttack:
		//コンボ攻撃の処理
		enemy_->ChangeState(new EnemyStateComboAttack());
		break;
	}
}

void EnemyStateDash::TransitionToRandomState()
{
	//アニメーションのブレンド時間をリセットし、ランダムな状態に遷移
	enemy_->GetAnimator()->SetBlendDuration(0.2f);
	int nextAction = RandomGenerator::GetRandomInt(0, 2);
	if (nextAction <= 1)
	{
		enemy_->ChangeState(new EnemyStateRoot());
	}
	else
	{
		enemy_->ChangeState(new EnemyStateDash());
	}
}

const Vector3 EnemyStateDash::CalculateEndPosition() const
{
	//敵の終了座標を計算
	Vector3 direction = { 0.0f, 0.0f, enemy_->GetDashParameters().dashDistance };
	return startPosition_ + Mathf::RotateVector(direction, enemy_->GetDestinationQuaternion());
}

float EnemyStateDash::GetEasingParameter() const
{
	//アニメーションの進行に応じたイージング係数を計算
	float currentAnimationTime = GetCurrentAnimationTime();
	float animationDuration = GetAnimationDuration();
	return currentAnimationTime / animationDuration;
}

const float EnemyStateDash::GetCurrentAnimationTime() const
{
	//アニメーションの進行時間を取得
	AnimatorComponent* animator = enemy_->GetAnimator();
	return animator->GetIsBlendingCompleted() ? animator->GetCurrentAnimationTime() : animator->GetNextAnimationTime();
}

const float EnemyStateDash::GetAnimationDuration() const
{
	//アニメーションの総時間を取得
	AnimatorComponent* animator = enemy_->GetAnimator();
	return animator->GetIsBlendingCompleted() ? animator->GetCurrentAnimationDuration() : animator->GetNextAnimationDuration();
}