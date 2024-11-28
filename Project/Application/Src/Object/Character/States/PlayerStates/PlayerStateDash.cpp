#include "PlayerStateDash.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Player/Player.h"

void PlayerStateDash::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションブレンドを有効にする
	character_->GetAnimator()->SetIsBlending(true);

	//ダッシュ開始時のアニメーションの再生とアニメーションコントローラーを取得
	SetAnimationControllerAndPlayAnimation(currentAnimation_);
}

void PlayerStateDash::Update()
{
	//アニメーションイベントを実行
	UpdateAnimationState();

	//現在のアニメーションがダッシュ終了かつアニメーションが終了していた場合
	if (character_->GetAnimator()->GetIsAnimationFinished())
	{	
		//現在のアニメーションがダッシュ開始の場合
		currentAnimation_ == "DashStart" ? CheckTransitionToDashEndAnimation() : HandleStateTransition();
	}
}

void PlayerStateDash::OnCollision(GameObject* other)
{
	//衝突処理
	character_->ProcessCollisionImpact(other, true);
	//ダッシュ終了処理
	FinalizeDash();
}

void PlayerStateDash::InitializeVelocityMovement(const VelocityMovementEvent* velocityMovementEvent, const int32_t animationEventIndex)
{
	//基底クラスの呼び出し
	IPlayerState::InitializeVelocityMovement(velocityMovementEvent, animationEventIndex);
	//プレイヤーを一時的に非表示にする
	character_->SetIsVisible(false);
	//武器を一時的に非表示にする
	character_->GetWeapon()->SetIsVisible(false);
}

void PlayerStateDash::CheckTransitionToDashEndAnimation()
{
	//ダッシュ終了処理
	FinalizeDash();

	//先行入力があった場合は状態を遷移してダッシュ攻撃のフラグを立てる
	if (CheckAndTransitionBufferedAction())
	{
		GetPlayer()->SetActionFlag(Player::ActionFlag::kDashAttackEnabled, true);
		return;
	}

	//現在のアニメーションの名前を変更
	currentAnimation_ = "DashEnd";

	//ダッシュ終了時のアニメーションの再生とアニメーションコントローラーを取得
	SetAnimationControllerAndPlayAnimation(currentAnimation_);
}

void PlayerStateDash::FinalizeDash()
{
	//キャラクターを表示
	character_->SetIsVisible(true);
	//武器を表示
	character_->GetWeapon()->SetIsVisible(true);
	//ポストエフェクトを無効化する
	PostEffects::GetInstance()->GetRadialBlur()->SetIsEnable(false);
}