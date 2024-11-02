#include "PlayerStateDodge.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateRoot.h"

void PlayerStateDodge::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//スティックの入力の強さを計算
	float inputLength = Mathf::Length({ input_->GetLeftStickX(), 0.0f, input_->GetLeftStickY() });

	//回避のアニメーションとアニメーションイベントを設定
	ConfigureDodgeAnimationAndEvents(inputLength);
}

void PlayerStateDodge::Update()
{
	//アニメーションによる座標のずれを修正
	character_->CorrectAnimationOffset();

	//アニメーションイベントを実行
	UpdateAnimationState();

	//ローリング回避の場合はプレイヤーを回転させる
	if (animationName_ == "DodgeForward")
	{
		character_->Rotate(Mathf::Normalize(processedVelocityData_.velocity));
	}

	//状態遷移
	HandleStateTransition();
}

void PlayerStateDodge::HandleStateTransition()
{
	//アニメーションが終了した場合
	if (character_->GetAnimator()->GetIsAnimationFinished())
	{
		//プレイヤーの位置をアニメーション後の位置に補正
		character_->CorrectPositionAfterAnimation();

		//通常状態に戻す
		GetPlayer()->ChangeState(new PlayerStateRoot());
	}
}

void PlayerStateDodge::ConfigureDodgeAnimationAndEvents(const float inputLength)
{
	//プレイヤーを取得
	Player* player = GetPlayer();

	//スティック入力の状態に応じてアニメーションとイベントを選択
	animationName_ = (inputLength > player->GetRootParameters().walkThreshold) ? "DodgeForward" : "DodgeBackward";

	//アニメーションイベントを設定
	animationController_ = &character_->GetEditorManager()->GetCombatAnimationEditor()->GetAnimationController("Player", animationName_);

	//アニメーションを再生
	character_->GetAnimator()->PlayAnimation(animationName_, 1.0f, false);
}