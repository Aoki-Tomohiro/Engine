#include "EnemyStateApproach.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"

void EnemyStateApproach::Initialize()
{
	//接近行動をランダムに決定
	SetRandomApproachAction();

	//アニメーションを再生
	SetAnimationControllerAndPlayAnimation(animationName_, animationName_ == "Run");
}

void EnemyStateApproach::Update()
{
	//走り状態ならプレイヤーの方向に回転させて移動ベクトルを回転させる
	if (animationName_ == "Run")
	{
		//プレイヤーの方向に向かせる
		GetEnemy()->LookAtPlayer();

		//移動ベクトルを回転させる
		RotateVelocity();
	}

	//アニメーションイベントを実行
	UpdateAnimationState();

	//状態遷移条件をチェックして遷移処理を実行
	if (character_->GetAnimator()->GetIsAnimationFinished() || IsOpponentInProximity())
	{
		HandleStateTransition();
	}
}

void EnemyStateApproach::OnCollision(GameObject* other)
{
	//衝突処理
	character_->ProcessCollisionImpact(other, false);
}

void EnemyStateApproach::SetRandomApproachAction()
{
	//接近行動をランダムに決定
	switch (static_cast<Enemy::ApproachType>(RandomGenerator::GetRandomInt(0, static_cast<int>(Enemy::ApproachType::MaxTypes) - 1)))
	{
	case Enemy::ApproachType::DashForward:
		//ダッシュ状態に設定
		animationName_ = "Dash";
		break;
	case Enemy::ApproachType::RunTowardsPlayer:
		//走り状態に設定
		animationName_ = "Run";
		break;
	}
}

void EnemyStateApproach::RotateVelocity()
{
	//移動イベントが存在しているかつアクティブ状態の場合
	if (!processedVelocityDatas_.empty() && processedVelocityDatas_[0].isActive)
	{
		//移動ベクトルを回転させる
		processedVelocityDatas_[0].velocity = Mathf::RotateVector(processedVelocityDatas_[0].velocity, character_->GetQuaternion());
	}
}