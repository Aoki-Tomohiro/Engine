#include "PlayerStateDodge.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"

void PlayerStateDodge::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//スティックの入力を取得
	Vector3 inputValue = {
		input_->GetLeftStickX(),
		0.0f,
		input_->GetLeftStickY(),
	};

	//スティック入力に応じて設定する
	if (Mathf::Length(inputValue) > player_->GetRootParameters().walkThreshold)
	{
		//前方への回避
		SetupDodge("DodgeForward", 2.4f);

		//速度を設定
		velocity_ = Mathf::Normalize(inputValue) * player_->GetDodgeParameters().dodgeSpeed;
		velocity_ = Mathf::RotateVector(velocity_, player_->GetDestinationQuaternion());

		//方向に応じてプレイヤーを回転させる
		player_->Rotate(Mathf::Normalize(velocity_));
	}
	else
	{
		//後方への回避
		SetupDodge("DodgeBackward", 2.0f);

		//速度を設定
		velocity_ = Mathf::RotateVector({ 0.0f,0.0f,-1.0f }, player_->GetDestinationQuaternion()) * player_->GetDodgeParameters().dodgeSpeed;
	}
}

void PlayerStateDodge::Update()
{
	//アニメーションによる座標のずれを修正
	player_->CorrectAnimationOffset();

	//アニメーションフェーズの更新
	UpdateAnimationPhase();

	//現在のフェーズが移動させる
	if (animationState_.phases[phaseIndex_].name == "DodgeMove")
	{
		player_->Move(velocity_);
	}

	//アニメーションが終了した場合
	if (player_->GetIsAnimationFinished())
	{
		//回避動作を完了する
		FinalizeDodge();
	}
}

void PlayerStateDodge::OnCollision(GameObject* other)
{
}

void PlayerStateDodge::SetupDodge(const std::string& animationName, float animationSpeed)
{
	//アニメーションの状態を取得して設定
	animationState_ = player_->GetCombatAnimationEditor()->GetAnimationState(animationName);

	//アニメーションの再生
	player_->PlayAnimation(animationName, animationSpeed, false);
}

void PlayerStateDodge::FinalizeDodge()
{
	//プレイヤーの位置をアニメーション後の位置に補正
	player_->SetIsAnimationCorrectionActive(true);
	player_->SetPosition(player_->GetPosition() + Vector3(player_->GetHipLocalPosition().x, 0.0f, player_->GetHipLocalPosition().z));

	//アニメーションを停止
	player_->StopAnimation();

	//通常状態に戻す
	player_->ChangeState(new PlayerStateRoot());
}

void PlayerStateDodge::UpdateAnimationPhase()
{
	//現在のアニメーション時間を取得
	float currentAnimationTime = player_->GetIsBlendingCompleted() ? player_->GetCurrentAnimationTime() : player_->GetNextAnimationTime();

	//次のフェーズがあり現在のアニメーション時間が現在のフェーズの持続時間を超えた場合フェーズを進める
	if (phaseIndex_ < animationState_.phases.size() - 1 && currentAnimationTime >= animationState_.phases[phaseIndex_].duration)
	{
		//フェーズを進める
		phaseIndex_++;
	}
}