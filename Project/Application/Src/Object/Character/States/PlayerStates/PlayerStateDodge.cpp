#include "PlayerStateDodge.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateRoot.h"

void PlayerStateDodge::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//スティックの入力を取得
	inputValue_ = {
		input_->GetLeftStickX(),
		0.0f,
		input_->GetLeftStickY(),
	};

	//スティック入力に応じて設定する
	if (Mathf::Length(inputValue_) > player_->GetRootParameters().walkThreshold)
	{
		//回避状態を設定
		dodgeState_ = DodgeState::kForward;

		//前方への回避
		SetupDodge("DodgeForward", 2.8f);
	}
	else
	{
		//回避状態を設定
		dodgeState_ = DodgeState::kBackward;

		//後方への回避
		SetupDodge("DodgeBackward", 2.0f);
	}
}

void PlayerStateDodge::Update()
{
	//アニメーションによる座標のずれを修正
	player_->CorrectAnimationOffset();

	//アニメーションフェーズの更新
	UpdateAnimationPhase();

	//移動処理
	player_->Move(velocity_);

	//アニメーションが終了した場合
	if (player_->GetAnimator()->GetIsAnimationFinished())
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
	player_->GetAnimator()->PlayAnimation(animationName, animationSpeed, false);
}

void PlayerStateDodge::FinalizeDodge()
{
	//プレイヤーの位置をアニメーション後の位置に補正
	player_->SetIsAnimationCorrectionActive(true);
	player_->SetPosition(player_->GetPosition() + Vector3(player_->GetJointLocalPosition("mixamorig:Hips").x, 0.0f, player_->GetJointLocalPosition("mixamorig:Hips").z));

	//アニメーションを停止
	player_->GetAnimator()->StopAnimation();

	//通常状態に戻す
	player_->ChangeState(new PlayerStateRoot());
}

void PlayerStateDodge::UpdateAnimationPhase()
{
	//アニメーターを取得
	AnimatorComponent* animator = player_->GetAnimator();

	//現在のアニメーション時間を取得
	float currentAnimationTime = animator->GetIsBlendingCompleted() ? animator->GetCurrentAnimationTime() : animator->GetNextAnimationTime();

	//次のフェーズがあり現在のアニメーション時間が現在のフェーズの持続時間を超えた場合フェーズを進める
	if (phaseIndex_ < animationState_.phases.size() - 1 && currentAnimationTime >= animationState_.phases[phaseIndex_].duration)
	{
		//フェーズを進める
		phaseIndex_++;

		//速度を計算
		CalculateVelocity();

		//回避状態になったら回転させる
		if (animationState_.phases[phaseIndex_].name == "DodgeMove" && dodgeState_ == DodgeState::kForward)
		{
			player_->Rotate(Mathf::Normalize(velocity_));
		}
	}
}

void PlayerStateDodge::CalculateVelocity()
{
	//回避状態に応じて速度の計算を変更
	switch (dodgeState_)
	{
	case DodgeState::kForward:
		//速度を計算
		velocity_ = Mathf::Normalize(inputValue_) * animationState_.phases[phaseIndex_].attackSettings.moveSpeed;
		velocity_ = Mathf::RotateVector(velocity_, player_->GetCamera()->quaternion_);
		velocity_.y = 0.0f;
		break;
	case DodgeState::kBackward:
		//速度を計算
		velocity_ = Mathf::RotateVector({ 0.0f,0.0f,-1.0f }, player_->GetDestinationQuaternion()) * animationState_.phases[phaseIndex_].attackSettings.moveSpeed;
		break;
	}
}