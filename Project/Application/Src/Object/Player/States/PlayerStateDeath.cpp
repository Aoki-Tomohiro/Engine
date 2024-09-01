#include "PlayerStateDeath.h"
#include "Application/Src/Object/Player/Player.h"

void PlayerStateDeath::Initialize()
{
	//死亡アニメーションを再生
	player_->PlayAnimation("Death", 1.0f, false);

	//ダメージのスプライトの色を初期化
	damagedSpriteColor_ = player_->GetDamagedSpriteColor();
	damagedSpriteColor_.w = 0.2f;
}

void PlayerStateDeath::Update()
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
}

void PlayerStateDeath::OnCollision(GameObject* other)
{
}