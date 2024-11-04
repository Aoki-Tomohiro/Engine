#include "PlayerStateDodge.h"
#include "Application/Src/Object/Character/Player/Player.h"

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

	//前方回避アニメーションの場合はキャラクターを回転
	ApplyRotationForForwardDodge();

	//デフォルトの状態に遷移
	if (character_->GetAnimator()->GetIsAnimationFinished())
	{
		HandleStateTransition(true);
	}
}

void PlayerStateDodge::ConfigureDodgeAnimationAndEvents(const float inputLength)
{
	//プレイヤーを取得
	Player* player = GetPlayer();

	//スティック入力の状態に応じてアニメーションとイベントを選択
	animationName_ = (inputLength > player->GetRootParameters().walkThreshold) ? "DodgeForward" : "DodgeBackward";

	//回避アニメーションの再生とアニメーションコントローラーを取得
	SetAnimationControllerAndPlayAnimation(animationName_);
}

void PlayerStateDodge::ApplyRotationForForwardDodge()
{
	//前方回避の場合はプレイヤーを回転させる
	if (animationName_ == "DodgeForward")
	{
		character_->Rotate(Mathf::Normalize(processedVelocityDatas_[0].velocity));
	}
}