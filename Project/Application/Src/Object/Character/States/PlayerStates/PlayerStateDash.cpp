#include "PlayerStateDash.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Player/Player.h"

void PlayerStateDash::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//ダッシュ開始時のアニメーションの再生とアニメーションコントローラーを取得
	SetAnimationControllerAndPlayAnimation(currentAnimation_);
}

void PlayerStateDash::Update()
{
	//アニメーションイベントを実行
	UpdateAnimationState();

	//ダッシュ終了アニメーションへの遷移チェック
	CheckTransitionToDashEndAnimation();

	//速度移動イベントが無効の場合はダッシュ終了処理
	if (processedVelocityDatas_.empty() || !processedVelocityDatas_[0].isActive)
	{
		FinalizeDash();
	}

	//現在のアニメーションがダッシュ終了かつアニメーションが終了していた場合
	if (character_->GetAnimator()->GetIsAnimationFinished() && currentAnimation_ == "DashEnd")
	{	
		//ダッシュ終了処理
		FinalizeDash();
		//デフォルトの状態に遷移
		HandleStateTransition();
	}
}

void PlayerStateDash::OnCollision(GameObject* other)
{
	//衝突処理
	character_->ProcessCollisionImpact(other, true);
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
	if (currentAnimation_ == "DashStart" && character_->GetAnimator()->GetIsAnimationFinished())
	{
		//先行入力があった場合はダッシュ状態のリセットをして状態を遷移
		if (CheckAndTransitionBufferedAction())
		{
			FinalizeDash();
			return;
		}
		//現在のアニメーションの名前を変更
		currentAnimation_ = "DashEnd";
		//ダッシュ終了時のアニメーションの再生とアニメーションコントローラーを取得
		SetAnimationControllerAndPlayAnimation(currentAnimation_);
	}
}

void PlayerStateDash::FinalizeDash()
{
	//キャラクターを表示
	character_->SetIsVisible(true);
	//武器を表示
	character_->GetWeapon()->SetIsVisible(true);
}