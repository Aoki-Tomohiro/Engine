#include "EnemyStateTackle.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRoot.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/MagicProjectile/MagicProjectile.h"
#include "Application/Src/Object/Player/Player.h"

void EnemyStateTackle::Initialize()
{
	//タックルアタックアニメーションを再生する
	enemy_->PlayAnimation("TackleAttack", 1.0f, false);

	//アニメーションの状態を取得して設定
	animationState_ = enemy_->GetCombatAnimationEditor()->GetAnimationState("TackleAttack");

	//プレイヤーへの方向ベクトルを計算し、敵を回転させる
	Vector3 directionToPlayer = GetDirectionToPlayer();
	enemy_->Rotate(directionToPlayer);

	//アタック設定に基づいて初期速度を設定
	velocity_ = directionToPlayer * animationState_.phases[0].attackSettings.moveSpeed;

	//武器に初期パラメータを適用
	ApplyParametersToWeapon();
}

void EnemyStateTackle::Update()
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

void EnemyStateTackle::OnCollision(GameObject* other)
{
	//衝突相手が武器だった場合
	if (Weapon* weapon = dynamic_cast<Weapon*>(other))
	{
		//HPを減らす
		enemy_->SetHP(enemy_->GetHP() - weapon->GetDamage());
	}
	//衝突相手が魔法だった場合
	else if (MagicProjectile* magicProjectile = dynamic_cast<MagicProjectile*>(other))
	{
		//HPを減らす
		enemy_->SetHP(enemy_->GetHP() - magicProjectile->GetDamage());
	}
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
		ApplyParametersToWeapon();
	}
}

void EnemyStateTackle::UpdateVelocityForCurrentPhase()
{
	//現在のフェーズに基づいて速度を更新
	velocity_ = Mathf::RotateVector({ 0.0f, 0.0f, animationState_.phases[phaseIndex_].attackSettings.moveSpeed }, enemy_->GetDestinationQuaternion());
}

void EnemyStateTackle::ApplyParametersToWeapon()
{
	//現在のフェーズの設定に基づいて武器のパラメータを適用
	Weapon* weapon = GameObjectManager::GetInstance()->GetMutableGameObject<Weapon>("EnemyWeapon");
	const auto& currentPhase = animationState_.phases[phaseIndex_];

	//ダメージを設定
	weapon->SetDamage(currentPhase.attackSettings.damage);

	//ヒットボックスを設定
	weapon->SetHitbox(currentPhase.hitbox);

	//エフェクトの設定を武器に設定
	weapon->SetEffectSettings(currentPhase.effectSettings);

	//ノックバックの設定を武器に設定
	KnockbackSettings knockbackSettings = currentPhase.knockbackSettings;
	knockbackSettings.knockbackVelocity = Mathf::RotateVector(knockbackSettings.knockbackVelocity, enemy_->GetDestinationQuaternion());
	knockbackSettings.knockbackAcceleration = Mathf::RotateVector(knockbackSettings.knockbackAcceleration, enemy_->GetDestinationQuaternion());
	weapon->SetKnockbackSettings(knockbackSettings);
}

void EnemyStateTackle::HandleCurrentPhase(Weapon* weapon)
{
	//現在のフェーズに応じて適切な処理を実行
	switch (phaseIndex_)
	{
	case kCharge:
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

void EnemyStateTackle::AttackUpdate(Weapon* weapon)
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

Vector3 EnemyStateTackle::GetDirectionToPlayer()
{
	//プレイヤーへの方向ベクトルを計算し、y成分を0に設定して正規化
	Vector3 directionToPlayer = GameObjectManager::GetInstance()->GetMutableGameObject<Player>("")->GetHipWorldPosition() - enemy_->GetHipWorldPosition();
	directionToPlayer.y = 0.0f;
	return Mathf::Normalize(directionToPlayer);
}