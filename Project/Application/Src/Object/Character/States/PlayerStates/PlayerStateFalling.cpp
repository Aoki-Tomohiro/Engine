/**
 * @file PlayerStateFalling.cpp
 * @brief プレイヤーの落下状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#include "PlayerStateFalling.h"
#include "Application/Src/Object/Character/Player/Player.h"

void PlayerStateFalling::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションブレンドを有効にする
	character_->GetAnimator()->SetIsBlending(true);

	//落下攻撃が有効かどうかを確認し、アニメーション停止時間を設定
	bool isInFallingAttackState = GetPlayer()->GetActionFlag(Player::ActionFlag::kFallingAttackEnabled);
	animationPauseThreshold_ = isInFallingAttackState ? fallingAttackPauseTime_ : fallingAnimationPauseTime_;

	//アニメーションを再生
	SetAnimationControllerAndPlayAnimation(isInFallingAttackState ? "FallingAttack" : "Falling");

	//落下攻撃が有効な場合はフラグをリセットし、アニメーション時間を設定
	if (isInFallingAttackState)
	{
		GetPlayer()->SetActionFlag(Player::ActionFlag::kFallingAttackEnabled, false);
		character_->GetAnimator()->SetNextAnimationTime(fallingAttackStartTime_);
	}
}

void PlayerStateFalling::Update()
{
	//アニメーションイベントを実行
	UpdateAnimationState();

	//重力を適用
	ApplyGravity();

	//空中または地面にいる場合の処理
	CheckAndHandleLanding();

	//デフォルトの状態に遷移
	if (character_->GetAnimator()->GetIsAnimationFinished())
	{
		HandleStateTransition();
	}
}

void PlayerStateFalling::OnCollision(GameObject* other)
{
	//衝突処理
	character_->ProcessCollisionImpact(other, false);
}

void PlayerStateFalling::ApplyGravity()
{
	//すでに着地している場合は重力を適用しない
	if (isCurrentlyLanding_) return;

	//速度移動イベントが存在しアクティブ状態であれば重力を適用
	if (!processedVelocityDatas_.empty() && processedVelocityDatas_[0].isActive)
	{
		Vector3 gravityForce = { 0.0f, character_->GetGravityAcceleration(), 0.0f };
		processedVelocityDatas_[0].velocity += gravityForce * GameTimer::GetDeltaTime() * character_->GetTimeScale();
	}
}

void PlayerStateFalling::CheckAndHandleLanding()
{
	//アニメーターを取得
	AnimatorComponent* animator = character_->GetAnimator();

	//現在のアニメーション時間を取得
	float currentAnimationTime = animator->GetIsBlendingCompleted() ? animator->GetCurrentAnimationTime() : animator->GetNextAnimationTime();

	//地面に到達した場合
	if (character_->GetPosition().y < character_->GetAdjustGroundLevel() && !isCurrentlyLanding_)
	{
		//着地フラグの設定
		isCurrentlyLanding_ = true;

		//速度を0にリセット
		processedVelocityDatas_[0].velocity = { 0.0f, 0.0f, 0.0f };
	}
	//空中状態の場合
	else if (currentAnimationTime >= animationPauseThreshold_)
	{
		//着地フラグの状態によってアニメーションを管理
		isCurrentlyLanding_ ? character_->GetAnimator()->ResumeAnimation() : character_->GetAnimator()->PauseAnimation();
	}
}