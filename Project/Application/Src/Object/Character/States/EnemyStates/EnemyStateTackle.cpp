#include "EnemyStateTackle.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"
#include "Application/Src/Object/Character/States/EnemyStates/EnemyStateRoot.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Character/Player/Player.h"

void EnemyStateTackle::Initialize()
{
	//タックルアタックアニメーションを再生する
	enemy_->GetAnimator()->PlayAnimation("TackleAttack", 1.0f, false);

	//アニメーションの状態を取得して設定
	animationState_ = enemy_->GetCombatAnimationEditor()->GetAnimationState("TackleAttack");

	//プレイヤーへの方向ベクトルを計算し、敵を回転させる
	Vector3 directionToPlayer = GetDirectionToPlayer();
	enemy_->Rotate(directionToPlayer);

	//アタック設定に基づいて初期速度を設定
	velocity_ = directionToPlayer * animationState_.phases[0].attackSettings.moveSpeed;

	//武器に初期パラメータを適用
	enemy_->ApplyParametersToWeapon("EnemyWeapon", animationState_.phases[phaseIndex_]);
}

void EnemyStateTackle::Update()
{
	//武器を取得
	Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>("EnemyWeapon");

	//アニメーターを取得
	AnimatorComponent* animator = enemy_->GetAnimator();

	//現在のアニメーション時間を取得
	float currentAnimationTime = animator->GetIsBlendingCompleted() ? animator->GetCurrentAnimationTime() : animator->GetNextAnimationTime();

	//アニメーションのフェーズを更新
	UpdateAnimationPhase(weapon, currentAnimationTime);

	//アニメーションのフェーズごとの処理
	HandleCurrentPhase(weapon);

	//アニメーションが終了したら通常状態に戻す
	if (enemy_->GetAnimator()->GetIsAnimationFinished())
	{
		enemy_->ChangeState(new EnemyStateRoot());
	}
}

void EnemyStateTackle::OnCollision(GameObject* other)
{
	//衝突処理
	enemy_->ProcessCollisionImpact(other, false);
}

void EnemyStateTackle::UpdateAnimationPhase(Weapon* weapon, float currentAnimationTime)
{
	//現在のアニメーション時間がフェーズの持続時間を超えた場合、次のフェーズに進む
	if (phaseIndex_ < animationState_.phases.size() - 1 && currentAnimationTime > animationState_.phases[phaseIndex_].duration)
	{
		//攻撃状態を無効にする
		weapon->SetIsAttack(false);

		//フェーズを進め、速度を再計算
		phaseIndex_++;
		UpdateVelocityForCurrentPhase();
		enemy_->ApplyParametersToWeapon("EnemyWeapon", animationState_.phases[phaseIndex_]);
	}
}

void EnemyStateTackle::UpdateVelocityForCurrentPhase()
{
	//現在のフェーズに基づいて速度を更新
	velocity_ = Mathf::RotateVector({ 0.0f, 0.0f, animationState_.phases[phaseIndex_].attackSettings.moveSpeed }, enemy_->GetDestinationQuaternion());
}

void EnemyStateTackle::HandleCurrentPhase(Weapon* weapon)
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
		AttackUpdate(weapon);
		break;
	case kRecovery:
		//攻撃の判定をなくす
		weapon->SetIsAttack(false);
		break;
	}
}

void EnemyStateTackle::ChargeUpdate()
{
	//プレイヤーへの方向ベクトルを計算し、敵を回転させる
	Vector3 directionToPlayer = GetDirectionToPlayer();
	enemy_->Rotate(directionToPlayer);
}

void EnemyStateTackle::AttackUpdate(Weapon* weapon)
{
	//プレイヤーと敵の座標を取得
	Vector3 playerPosition = GameObjectManager::GetInstance()->GetGameObject<Player>("Player")->GetJointWorldPosition("mixamorig:Hips");
	Vector3 enemyPosition = enemy_->GetJointWorldPosition("mixamorig:Hips");

	//プレイヤーへの方向ベクトルを計算
	Vector3 directionToPlayer = playerPosition - enemyPosition;
	directionToPlayer.y = 0.0f;

	//移動が終了していない場合
	if (!isMovementFinished)
	{
		//プレイヤーからの距離が停止距離より大きい場合、移動を続ける
		if (Mathf::Length(directionToPlayer) > enemy_->GetRootParameters().stopDistance)
		{
			enemy_->Move(velocity_);
		}
		else
		{
			//移動を終了させる
			isMovementFinished = true;
		}
	}

	//攻撃がヒットしていない場合
	if (!isHit_)
	{
		//武器の衝突判定を付ける
		weapon->SetIsAttack(true);

		//攻撃が当たっていたら
		if (weapon->GetIsHit())
		{
			isHit_ = true;
			weapon->SetIsAttack(false);
		}
	}
}

Vector3 EnemyStateTackle::GetDirectionToPlayer()
{
	//プレイヤーへの方向ベクトルを計算し、y成分を0に設定して正規化
	Vector3 directionToPlayer = GameObjectManager::GetInstance()->GetGameObject<Player>("Player")->GetJointWorldPosition("mixamorig:Hips") - enemy_->GetJointWorldPosition("mixamorig:Hips");
	directionToPlayer.y = 0.0f;
	return Mathf::Normalize(directionToPlayer);
}