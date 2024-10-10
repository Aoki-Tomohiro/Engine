#include "PlayerStateDeath.h"
#include "Application/Src/Object/Character/Player/Player.h"

void PlayerStateDeath::Initialize()
{
	//死亡アニメーションを再生
	player_->GetAnimator()->PlayAnimation("Death", 1.0f, false);
}

void PlayerStateDeath::Update()
{
	//アニメーターを取得
	AnimatorComponent* animator = player_->GetAnimator();

	//現在のアニメーション時間を取得
	float animationTime = animator->GetIsBlendingCompleted() ? animator->GetCurrentAnimationTime() : animator->GetNextAnimationTime();

	//アニメーションの持続時間を取得
	float animationDuration = animator->GetIsBlendingCompleted() ? animator->GetCurrentAnimationDuration() : animator->GetNextAnimationDuration();

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