#include "EnemyStateStun.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRoot.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/MagicProjectile/MagicProjectile.h"

void EnemyStateStun::Initialize()
{
	//アニメーションブレンドを無効にする
	enemy_->SetIsAnimationBlending(false);

	//現在のリアクションタイプを取得
	currentReactionType_ = enemy_->GetKnockbackSettings().reactionType_;

	//アニメーションを再生する
	PlayReactionAnimation(currentReactionType_, 2.0f);
}

void EnemyStateStun::Update()
{
	//ノックバックの処理
	enemy_->ApplyKnockback();

	//地面に埋まらないように補正
	AdjustEnemyPosition();

	//スタンが終了している場合
	if (isStunFinished_)
	{
		if (enemy_->GetIsBlendingCompleted() && enemy_->GetIsAnimationFinished())
		{
			RecoverFromStun();
		}
		return;
	}

	//アニメーションが終了している場合
	if (enemy_->GetIsAnimationFinished() && enemy_->GetPosition().y == 0.0f)
	{
		isStunFinished_ = true;

		//リアクションタイプに応じて復帰処理を行う
		if (currentReactionType_ == ReactionType::Flinch)
		{
			RecoverFromStun();
		}
		else if (currentReactionType_ == ReactionType::Knockback)
		{
			enemy_->SetIsAnimationBlending(true);
			enemy_->SetBlendDuration(1.0f);
			enemy_->PlayAnimation("StandUp", 3.0f, false);
		}
	}
}

void EnemyStateStun::OnCollision(GameObject* other)
{
	//衝突相手が武器だった場合
	if (Weapon* weapon = dynamic_cast<Weapon*>(other))
	{
		ApplyDamageAndKnockback(weapon->GetDamage(), weapon->GetKnockbackSettings(), true);
	}
	//衝突相手が魔法だった場合
	else if (MagicProjectile* magicProjectile = dynamic_cast<MagicProjectile*>(other))
	{
		ApplyDamageAndKnockback(magicProjectile->GetDamage(), magicProjectile->GetKnockbackSettings(), magicProjectile->GetMagicType() == MagicProjectile::MagicType::kCharged);
	}
}

void EnemyStateStun::PlayReactionAnimation(ReactionType reactionType, float speed)
{
	//リアクションタイプに合わせてアニメーションを再生させる
	switch (reactionType)
	{
	case ReactionType::Flinch:
		enemy_->PlayAnimation("HitStun", speed, false);
		break;
	case ReactionType::Knockback:
		enemy_->PlayAnimation("Knockdown", speed, false);
		break;
	}
	currentReactionType_ = reactionType;
}

void EnemyStateStun::AdjustEnemyPosition()
{
	//敵の座標を取得
	Vector3 position = enemy_->GetPosition();

	//地面に埋まらないようにする
	if (position.y < 0.0f)
	{
		position.y = 0.0f;
		enemy_->SetPosition(position);
	}
}

void EnemyStateStun::RecoverFromStun()
{
	enemy_->SetBlendDuration(0.6f);
	enemy_->SetIsStunRecovered(true);
	enemy_->ChangeState(new EnemyStateRoot());
}

void EnemyStateStun::ApplyDamageAndKnockback(const float damage, const KnockbackSettings& knockbackSettings, const bool applyKnockback)
{
	//HPを減らす
	enemy_->SetHP(enemy_->GetHP() - damage);

	//モデルシェイクを開始
	enemy_->StartModelShake();

	//ノックバックを適用してアニメーションを再生させる
	if (applyKnockback)
	{
		ApplyKnockbackAndPlayStunAnimation(knockbackSettings);
	}
}

void EnemyStateStun::ApplyKnockbackAndPlayStunAnimation(const KnockbackSettings& knockbackSettings)
{
	//スタンが終了していた場合は処理を飛ばす
	if (isStunFinished_) return;

	//ノックバック設定を適用
	enemy_->SetKnockbackSettings(knockbackSettings);

	//リアクションのタイプを設定
	ReactionType reactionType = ReactionType::Flinch;
	switch (knockbackSettings.reactionType_)
	{
	case ReactionType::Flinch:
		reactionType = currentReactionType_ == ReactionType::Knockback ? ReactionType::Knockback : ReactionType::Flinch;
		break;
	case ReactionType::Knockback:
		reactionType = knockbackSettings.reactionType_;
		break;
	}

	//アニメーションを再生
	PlayReactionAnimation(reactionType, 2.0f);
}