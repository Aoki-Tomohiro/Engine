#include "PlayerStateStun.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateRoot.h"

void PlayerStateStun::Initialize()
{
	//アニメーションブレンドを無効にする
	player_->GetAnimator()->SetIsBlending(false);

	//アニメーションを再生する
	player_->GetAnimator()->PlayAnimation("Impact", 2.0f, false);
}

void PlayerStateStun::Update()
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

	if (animator->GetIsAnimationFinished())
	{
		//通常状態に戻す
		player_->ChangeState(new PlayerStateRoot());
	}
}