#include "EnemyStateJumpAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRoot.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/MagicProjectile/MagicProjectile.h"

void EnemyStateJumpAttack::Initialize()
{
	//ジャンプ攻撃アニメーションを再生する
	enemy_->PlayAnimation("JumpAttack", 1.0f, false);

	//アニメーションの状態を取得して設定
	animationState_ = enemy_->GetCombatAnimationEditor()->GetAnimationState("JumpAttack");

	//武器に初期パラメータを適用
	ApplyParametersToWeapon();
}

void EnemyStateJumpAttack::Update()
{
	//武器と現在のアニメーション時間を取得
	Weapon* weapon = GameObjectManager::GetInstance()->GetMutableGameObject<Weapon>("EnemyWeapon");
	float currentAnimationTime = enemy_->GetIsBlendingCompleted() ? enemy_->GetCurrentAnimationTime() : enemy_->GetNextAnimationTime();

	//アニメーションのフェーズを更新
	UpdateAnimationPhase(weapon, currentAnimationTime);

	//アニメーションのフェーズごとの処理
	HandleCurrentPhase(weapon);

	//アニメーションが終了した場合
	if (enemy_->GetIsAnimationFinished())
	{
		//敵の位置をアニメーション後の位置に補正
		enemy_->SetIsAnimationCorrectionActive(true);
		enemy_->SetPosition(enemy_->GetPosition() + Vector3(enemy_->GetHipLocalPosition().x, 0.0f, enemy_->GetHipLocalPosition().z));

		//アニメーションを停止
		enemy_->StopAnimation();

		//通常状態に戻す
		enemy_->ChangeState(new EnemyStateRoot());
	}
}

void EnemyStateJumpAttack::OnCollision(GameObject* other)
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

void EnemyStateJumpAttack::UpdateAnimationPhase(Weapon* weapon, float currentAnimationTime)
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

void EnemyStateJumpAttack::UpdateVelocityForCurrentPhase()
{
	//現在のフェーズに基づいて速度を更新
	velocity_ = Mathf::RotateVector({ 0.0f, 0.0f, animationState_.phases[phaseIndex_].attackSettings.moveSpeed }, enemy_->GetDestinationQuaternion());
}

void EnemyStateJumpAttack::ApplyParametersToWeapon()
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

void EnemyStateJumpAttack::HandleCurrentPhase(Weapon* weapon)
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

void EnemyStateJumpAttack::AttackUpdate(Weapon* weapon)
{
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