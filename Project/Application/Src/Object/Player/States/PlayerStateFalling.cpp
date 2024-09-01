#include "PlayerStateFalling.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Player/States/PlayerStateStun.h"
#include "Application/Src/Object/Weapon/Weapon.h"

void PlayerStateFalling::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションの状態の取得
	animationState_ = player_->GetCombatAnimationEditor()->GetAnimationState("Falling");

	//アニメーションブレンドを有効化する
	player_->SetIsAnimationBlending(true);

	//落下アニメーションを再生
	player_->PlayAnimation("Falling", 1.0f, false);
}

void PlayerStateFalling::Update()
{
	//アニメーションによる座標のずれを補正
	player_->CorrectAnimationOffset();

	//プレイヤーが着地している場合の処理
	if (isLanding_)
	{
		HandleLanding();
		return;
	}

	//アニメーションの更新
	CheckAndPauseAnimation();

	//プレイヤーの落下処理
	ApplyGravity();

	//プレイヤーが地面についたらを確認
	CheckLanding();
}

void PlayerStateFalling::OnCollision(GameObject* other)
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

		//スタン状態に遷移
		player_->ChangeState(new PlayerStateStun());
	}
}

void PlayerStateFalling::ApplyGravity()
{
	//重力加速度ベクトルの設定
	Vector3 accelerationVector = { 0.0f, player_->GetJumpParameters().gravityAcceleration, 0.0f };

	//速度に重力加速度を加算
	velocity_ += accelerationVector * GameTimer::GetDeltaTime();

	//移動処理
	player_->Move(velocity_);
}

void PlayerStateFalling::CheckAndPauseAnimation()
{
	//現在のアニメーション時間を取得
	float animationTime = player_->GetIsBlendingCompleted() ? player_->GetCurrentAnimationTime() : player_->GetNextAnimationTime();

	//アニメーション時間が指定したフェーズの終了時間を超えた場合
	if (animationTime > animationState_.phases[0].duration)
	{
		//落下アニメーションを一時停止
		player_->PauseAnimation();
	}
}

void PlayerStateFalling::CheckLanding()
{
	//プレイヤーの座標を取得
	Vector3 position = player_->GetPosition();

	//プレイヤーが地面についた場合
	if (position.y <= 0.0f)
	{
		//着地フラグを立てる
		isLanding_ = true;

		//プレイヤーが地面に埋まらないように座標を補正
		position.y = 0.0f;
		player_->SetPosition(position);

		//落下アニメーションを一時停止を解除
		player_->ResumeAnimation();
	}
}

void PlayerStateFalling::HandleLanding()
{
	//アニメーションの再生が終了していたら通常状態に戻す
	if (Mathf::Length(GetInputValue()) > player_->GetRootParameters().walkThreshold || player_->GetIsAnimationFinished())
	{
		player_->ChangeState(new PlayerStateRoot());
	}
}

Vector3 PlayerStateFalling::GetInputValue()
{
	//スティックの入力を取得
	return { input_->GetLeftStickX(), 0.0f, input_->GetLeftStickY() };
}