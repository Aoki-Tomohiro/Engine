#include "EnemyStateLaserBeam.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"
#include "Application/Src/Object/Character/States/EnemyStates/EnemyStateRoot.h"
#include "Application/Src/Object/Character/Player/Player.h"

void EnemyStateLaserBeam::Initialize()
{
	//地面から突起が出る魔法のアニメーションを再生する
	enemy_->GetAnimator()->PlayAnimation("LaserBeam", 1.0f, false);

	//アニメーションの状態を取得して設定
	animationState_ = enemy_->GetCombatAnimationEditor()->GetAnimationState("LaserBeam");

	//プレイヤーへの方向ベクトルを計算し、敵を回転させる
	Vector3 directionToPlayer = GetDirectionToPlayer();
	enemy_->Rotate(directionToPlayer);
}

void EnemyStateLaserBeam::Update()
{
	//アニメーションのフェーズごとの処理
	HandleCurrentPhase();

	//アニメーターを取得
	AnimatorComponent* animator = enemy_->GetAnimator();

	//現在のアニメーション時間を取得
	float currentAnimationTime = animator->GetIsBlendingCompleted() ? animator->GetCurrentAnimationTime() : animator->GetNextAnimationTime();

	//アニメーションのフェーズを更新
	UpdateAnimationPhase(currentAnimationTime);

	//アニメーションが終了したら通常状態に戻す
	if (animator->GetIsAnimationFinished())
	{
		enemy_->ChangeState(new EnemyStateRoot());
	}
}

void EnemyStateLaserBeam::OnCollision(GameObject* other)
{
	//衝突処理
	enemy_->ProcessCollisionImpact(other, false);
}

void EnemyStateLaserBeam::UpdateAnimationPhase(const float currentAnimationTime)
{
	//現在のアニメーション時間がフェーズの持続時間を超えた場合、次のフェーズに進む
	if (phaseIndex_ < animationState_.phases.size() - 1 && currentAnimationTime > animationState_.phases[phaseIndex_].duration)
	{
		//フェーズを進める
		phaseIndex_++;

		//現在のフェーズが攻撃状態になった場合
		if (animationState_.phases[phaseIndex_].name == "Attack")
		{
			//回転の補間速度を遅くする
			enemy_->SetQuaternionInterpolationSpeed(0.06f);

			//レーザーを生成する
			CreateLaser();
		}
		else
		{
			//回転の補間速度を元に戻す
			enemy_->SetQuaternionInterpolationSpeed(0.3f);
		}
	}
}

void EnemyStateLaserBeam::CreateLaser()
{
	//レーザーを生成する
	laser_ = GameObjectManager::CreateGameObject<Laser>("Laser");
	//レーザーの初期化
	laser_->SetTargetScale({ 1.0f,1.0f,enemy_->GetLaserBeamParameters().laserRange });
	laser_->SetLifeTime(animationState_.phases[2].duration - animationState_.phases[1].duration);
	laser_->SetEasingSpeed(enemy_->GetLaserBeamParameters().easingSpeed);
	laser_->SetDamage(enemy_->GetLaserBeamParameters().damage);
}

void EnemyStateLaserBeam::HandleCurrentPhase()
{
	//現在のフェーズに応じて適切な処理を実行
	switch (phaseIndex_)
	{
	case kCharge:
		//チャージフェーズの処理
		ChargeUpdate();
		break;
	case kAttack:
		//攻撃フェーズの処理
		AttackUpdate();
		break;
	}
}

void EnemyStateLaserBeam::ChargeUpdate()
{
	//プレイヤーの方向に回転させる
	Vector3 directionToPlayer = GetDirectionToPlayer();
	enemy_->Rotate(directionToPlayer);
}

void EnemyStateLaserBeam::AttackUpdate()
{
	//プレイヤーの方向に回転させる
	Vector3 directionToPlayer = GetDirectionToPlayer();
	enemy_->Rotate(directionToPlayer);

	//レーザーの座標を更新する
	Vector3 laserPosition = enemy_->GetPosition() + Mathf::RotateVector(Vector3{ 0.0f, 0.0f, enemy_->GetLaserBeamParameters().laserRange } + enemy_->GetLaserBeamParameters().laserOffset, enemy_->GetQuaternion());
	laser_->SetPosition(laserPosition);

	//レーザーの回転を適用
	laser_->SetRotation(enemy_->GetQuaternion());
}

Vector3 EnemyStateLaserBeam::GetDirectionToPlayer()
{
	//プレイヤーへの方向ベクトルを計算し、y成分を0に設定して正規化
	Vector3 directionToPlayer = GameObjectManager::GetInstance()->GetGameObject<Player>("Player")->GetJointWorldPosition("mixamorig:Hips") - enemy_->GetJointWorldPosition("mixamorig:Hips");;
	directionToPlayer.y = 0.0f;
	return Mathf::Normalize(directionToPlayer);
}