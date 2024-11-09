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
	//アニメーションイベントを実行
	UpdateAnimationState();

	//デフォルトの状態に遷移
	if (character_->GetAnimator()->GetIsAnimationFinished())
	{
		HandleStateTransition();
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