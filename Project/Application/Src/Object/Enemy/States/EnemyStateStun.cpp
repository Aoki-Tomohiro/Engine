#include "EnemyStateStun.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRoot.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/MagicProjectile/MagicProjectile.h"

void EnemyStateStun::Initialize()
{
	//アニメーションブレンドを無効にする
	enemy_->SetIsAnimationBlending(false);

	//アニメーションを再生する
	enemy_->PlayAnimation("Stun", 2.0f, false);
}

void EnemyStateStun::Update()
{
	//ノックバックの処理
	enemy_->ApplyKnockback();

	//敵の座標を取得
	Vector3 position = enemy_->GetPosition();

	//敵が地面についた場合
	if (position.y < 0.0f)
	{
		//プレイヤーが地面に埋まらないように座標を補正
		position.y = 0.0f;
		enemy_->SetPosition(position);
	}

	//アニメーションが終了した場合
	if (enemy_->GetIsAnimationFinished() && position.y == 0.0f)
	{
		//敵の位置をアニメーション後の位置に補正
		enemy_->SetIsAnimationCorrectionActive(true);
		enemy_->SetPosition(enemy_->GetPosition() + Vector3(enemy_->GetHipLocalPosition().x, 0.0f, enemy_->GetHipLocalPosition().z));

		//通常状態に戻す
		enemy_->ChangeState(new EnemyStateRoot());
	}
}

void EnemyStateStun::OnCollision(GameObject* other)
{
	//衝突相手が武器だった場合
	if (Weapon* weapon = dynamic_cast<Weapon*>(other))
	{
		//ノックバックの速度を設定
		enemy_->SetKnockbackSettings(weapon->GetKnockbackSettings());

		//HPを減らす
		enemy_->SetHP(enemy_->GetHP() - weapon->GetDamage());

		//アニメーションを再生する
		enemy_->PlayAnimation("Stun", 2.0f, false);
	}
	//衝突相手が魔法だった場合
	else if (MagicProjectile* magicProjectile = dynamic_cast<MagicProjectile*>(other))
	{
		//HPを減らす
		enemy_->SetHP(enemy_->GetHP() - magicProjectile->GetDamage());

		//チャージマジックの場合
		if (magicProjectile->GetMagicType() == MagicProjectile::MagicType::kCharged)
		{
			//ノックバックの速度を設定
			enemy_->SetKnockbackSettings(magicProjectile->GetKnockbackSettings());

			//アニメーションを再生する
			enemy_->PlayAnimation("Stun", 2.0f, false);
		}
	}
}