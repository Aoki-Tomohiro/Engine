#include "EnemyStateComboAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRoot.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Player/Player.h"

void EnemyStateComboAttack::Initialize()
{
	//コンボ攻撃アニメーションを再生する
	enemy_->PlayAnimation("ComboAttack", 1.0f, false);

	//アニメーションの状態を取得して設定
	animationState_ = enemy_->GetCombatAnimationEditor()->GetAnimationState("ComboAttack");

	//プレイヤーへの方向ベクトルを計算し、敵を回転させる
	Vector3 directionToPlayer = GetDirectionToPlayer();
	enemy_->Rotate(directionToPlayer);

	//アタック設定に基づいて初期速度を設定
	velocity_ = directionToPlayer * animationState_.phases[0].attackSettings.moveSpeed;

	//武器に初期パラメータを適用
	enemy_->ApplyParametersToWeapon(animationState_.phases[phaseIndex_]);
}

void EnemyStateComboAttack::Update()
{
	//武器と現在のアニメーション時間を取得
	Weapon* weapon = GameObjectManager::GetInstance()->GetMutableGameObject<Weapon>("EnemyWeapon");
	float currentAnimationTime = enemy_->GetIsBlendingCompleted() ? enemy_->GetCurrentAnimationTime() : enemy_->GetNextAnimationTime();

	//アニメーションのフェーズを更新
	UpdateAnimationPhase(weapon, currentAnimationTime);

	//アニメーションのフェーズごとの処理
	HandleCurrentPhase(weapon);

	//アニメーションが終了したら通常状態に戻す
	if (enemy_->GetIsAnimationFinished())
	{
		enemy_->ChangeState(new EnemyStateRoot());
	}
}

void EnemyStateComboAttack::OnCollision(GameObject* other)
{
	//衝突処理
	enemy_->ProcessCollisionImpact(other, false);
}

void EnemyStateComboAttack::UpdateAnimationPhase(Weapon* weapon, float currentAnimationTime)
{
	//現在のアニメーション時間がフェーズの持続時間を超えた場合、次のフェーズに進む
	if (phaseIndex_ < animationState_.phases.size() - 1 && currentAnimationTime > animationState_.phases[phaseIndex_].duration)
	{
		//攻撃状態を無効にする
		weapon->SetIsAttack(false);

		//フェーズを進め、速度を再計算
		phaseIndex_++;
		UpdateVelocityForCurrentPhase();
		enemy_->ApplyParametersToWeapon(animationState_.phases[phaseIndex_]);
	}
}

void EnemyStateComboAttack::RotateTowardsPlayer()
{
	//プレイヤーへの方向を計算し、敵をプレイヤーの方向に回転
	Vector3 directionToPlayer = GetDirectionToPlayer();
	enemy_->Rotate(directionToPlayer);
}

void EnemyStateComboAttack::UpdateVelocityForCurrentPhase()
{
	//現在のフェーズに基づいて速度を更新
	velocity_ = Mathf::RotateVector({ 0.0f, 0.0f, animationState_.phases[phaseIndex_].attackSettings.moveSpeed }, enemy_->GetDestinationQuaternion());
}

void EnemyStateComboAttack::HandleCurrentPhase(Weapon* weapon)
{
	//現在のフェーズに応じて適切な処理を実行
	switch (phaseIndex_)
	{
	case kCharge1:
	case kCharge2:
	case kCharge3:
		//溜めフェーズの更新
		ChargeUpdate();
		break;
	case kAttack1:
	case kAttack2:
	case kAttack3:
		//攻撃フェーズの処理
		AttackUpdate(weapon);
		break;
	case kRecovery:
		//攻撃の判定をなくす
		weapon->SetIsAttack(false);
		break;
	}
}

void EnemyStateComboAttack::ChargeUpdate()
{
	//フラグの初期化
	isMovementFinished = false;
	isHit_ = false;

	//プレイヤーの方向に回転させる
	RotateTowardsPlayer();
}

void EnemyStateComboAttack::AttackUpdate(Weapon* weapon)
{
	//プレイヤーと敵の座標を取得
	Vector3 playerPosition = GameObjectManager::GetInstance()->GetMutableGameObject<Player>("")->GetHipWorldPosition();
	Vector3 enemyPosition = enemy_->GetHipWorldPosition();

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

Vector3 EnemyStateComboAttack::GetDirectionToPlayer()
{
	//プレイヤーへの方向ベクトルを計算し、y成分を0に設定して正規化
	Vector3 directionToPlayer = GameObjectManager::GetInstance()->GetMutableGameObject<Player>("")->GetHipWorldPosition() - enemy_->GetHipWorldPosition();
	directionToPlayer.y = 0.0f;
	return Mathf::Normalize(directionToPlayer);
}