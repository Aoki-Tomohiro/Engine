#include "PlayerStateDodge.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"

void PlayerStateDodge::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションブレンドを有効にする
	character_->GetAnimator()->SetIsBlending(true);

	//アニメーションブレンドの時間を設定
	character_->GetAnimator()->SetBlendDuration(0.2f);

	//コライダーを無効化する
	character_->GetCollider()->SetCollisionEnabled(false);

	//スティックの入力の強さを計算
	float inputLength = Mathf::Length({ input_->GetLeftStickX(), 0.0f, input_->GetLeftStickY() });

	//回避のアニメーションとアニメーションイベントを設定
	ConfigureDodgeAnimationAndEvents(inputLength);

	//敵がジャスト回避受付中の場合はジャスト回避成功
	if (GameObjectManager::GetInstance()->GetGameObject<Enemy>("Enemy")->GetIsVulnerableToPerfectDodge())
	{
		GetPlayer()->SetActionFlag(Player::ActionFlag::kJustDodge, true);
		GameTimer::SetTimeScale(0.2f);
	}
}

void PlayerStateDodge::Update()
{
	//アニメーションイベントを実行
	UpdateAnimationState();

	//ジャスト回避演出の更新
	UpdateJustDodgeEffect();

	//状態遷移されていた場合はコライダーを有効化する
	if (HasStateTransitioned())
	{
		//コライダーを有効化する
		character_->GetCollider()->SetCollisionEnabled(true);
	}

	//アニメーションが終了していた場合
	if (character_->GetAnimator()->GetIsAnimationFinished())
	{
		//コライダーを有効化する
		character_->GetCollider()->SetCollisionEnabled(true);
		//デフォルトの状態に遷移
		HandleStateTransition();
	}
}

void PlayerStateDodge::UpdateJustDodgeEffect()
{
	//ジャスト回避が成功している場合
	if (GetPlayer()->GetActionFlag(Player::ActionFlag::kJustDodge))
	{
		//ジャスト回避演出用のタイマーを進める
		const float kDeltaTime = 1.0f / 60.0f;
		justDodgeTimer_ += kDeltaTime;

		//演出終了条件（タイマーが一定値を超えた場合）
		const float kJustDodgeEffectDuration = 0.2f;
		if (justDodgeTimer_ > kJustDodgeEffectDuration)
		{
			//フラグをリセットして、タイムスケールを元に戻す
			GetPlayer()->SetActionFlag(Player::ActionFlag::kJustDodge, false);
			GameTimer::SetTimeScale(1.0f);
		}
	}
}

void PlayerStateDodge::ConfigureDodgeAnimationAndEvents(const float inputLength)
{
	//スティック入力の状態に応じてアニメーションとイベントを選択
	animationName_ = (inputLength > GetPlayer()->GetRootParameters().walkThreshold) ? "DodgeForward" : "DodgeBackward";

	//回避アニメーションの再生とアニメーションコントローラーを取得
	SetAnimationControllerAndPlayAnimation(animationName_);
}