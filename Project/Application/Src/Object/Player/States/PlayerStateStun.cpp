#include "PlayerStateStun.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Weapon/Weapon.h"

void PlayerStateStun::Initialize()
{
	//アニメーションを再生する
	player_->PlayAnimation("Impact", 2.0f, false);

	//ダメージのスプライトの色を初期化
	damagedSpriteColor_ = player_->GetDamagedSpriteColor();
	damagedSpriteColor_.w = 0.2f;
}

void PlayerStateStun::Update()
{
	//現在のアニメーション時間を取得
	float animationTime = player_->GetIsBlendingCompleted() ? player_->GetCurrentAnimationTime() : player_->GetNextAnimationTime();

	//アニメーションの持続時間を取得
	float animationDuration = player_->GetIsBlendingCompleted() ? player_->GetAnimationDuration() : player_->GetNextAnimationDuration();

	//スプライトの色を徐々に薄くする
	damagedSpriteColor_.w = 0.2f - (0.2f * (animationTime / animationDuration));
	damagedSpriteColor_.w = std::max<float>(0.0f, damagedSpriteColor_.w);
	player_->SetDamagedSpriteColor(damagedSpriteColor_);

	//ノックバックの処理
	player_->ApplyKnockback();

	//プレイヤーの座標を取得
	Vector3 position = player_->GetPosition();

	//プレイヤーが地面についた場合
	if (position.y <= 0.0f)
	{
		//プレイヤーが地面に埋まらないように座標を補正
		position.y = 0.0f;
		player_->SetPosition(position);
	}

	if (player_->GetIsAnimationFinished())
	{
		//通常状態に戻す
		player_->ChangeState(new PlayerStateRoot());
	}
}

void PlayerStateStun::OnCollision(GameObject* other)
{
	//衝突相手が武器だった場合
	if (Weapon* weapon = dynamic_cast<Weapon*>(other))
	{
		//ノックバックの速度を設定
		player_->SetKnockbackSettings(weapon->GetKnockbackSettings());

		//HPを減らす
		player_->SetHP(player_->GetHP() - weapon->GetDamage());

		//ダメージを食らった音を再生
		player_->PlayDamageSound();

		//スプライトの色の設定
		damagedSpriteColor_.w = 0.2f;

		//アニメーションを再生する
		player_->PlayAnimation("Impact", 2.0f, false);
	}
}