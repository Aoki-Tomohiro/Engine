#include "PlayerStateDodge.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"

void PlayerStateDodge::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションブレンドを無効化する
	player_->SetIsAnimationBlending(false);

	//プレイヤーの現在の座標を設定
	startPosition_ = player_->GetPosition();

	//スティックの入力を取得
	Vector3 inputValue = {
		input_->GetLeftStickX(),
		0.0f,
		input_->GetLeftStickY(),
	};

	//回避動作を行うかどうかを判定
	if (Mathf::Length(inputValue) > player_->GetRootParameters().walkThreshold)
	{
		//回避アニメーションの再生
		PlayDodgeAnimation(inputValue);

		//目標座標を設定
		targetPosition_ = startPosition_ + CalculateDodgeDistance(inputValue);
	}
	else
	{
		//後退アニメーションを再生
		player_->PlayAnimation("Dodge2", 1.6f, false);

		//後退距離を計算して目標座標を設定
		targetPosition_ = startPosition_ + CalculateFallbackDistance();
	}
}

void PlayerStateDodge::Update()
{
	//イージングの係数を計算
	float easingParameter = CalculateEasingParameter();

	//移動後の座標を計算
	Vector3 movedPosition = InterpolatePosition(easingParameter);

	//アニメーションによる座標のずれを修正
	player_->CorrectAnimationOffset();

	//プレイヤーに座標を設定
	player_->SetPosition(movedPosition);

	//アニメーションが終了した場合
	if (player_->GetIsAnimationFinished())
	{
		//回避動作を完了する
		FinalizeDodge();
	}
}

void PlayerStateDodge::PlayDodgeAnimation(const Vector3& inputValue)
{
	if (player_->GetLockon()->ExistTarget())
	{
		//ロックオン中の場合、入力方向に応じた回避アニメーションを再生
		PlayDirectionalDodgeAnimation(inputValue);
	}
	else
	{
		//通常の前進回避アニメーションを再生
		player_->PlayAnimation("Dodge1", 1.6f, false);
	}
}

void PlayerStateDodge::PlayDirectionalDodgeAnimation(const Vector3& inputValue)
{
	//入力値の絶対値を取得
	float horizontalValue = std::abs(inputValue.x);
	float verticalValue = std::abs(inputValue.z);

	//垂直方向の移動量が大きい場合前進または後退アニメーションを再生
	if (verticalValue >= horizontalValue)
	{
		player_->PlayAnimation(inputValue.z > 0.0f ? "Dodge1" : "Dodge2", 1.6f, false);
	}
	//水平方向の移動量が大きい場合、左または右移動のアニメーションを再生
	else
	{
		player_->PlayAnimation(inputValue.x < 0.0f ? "Dodge3" : "Dodge4", 1.6f, false);
	}
}

Vector3 PlayerStateDodge::CalculateDodgeDistance(const Vector3& inputValue) const
{
	Vector3 distance = Mathf::Normalize(inputValue) * player_->GetDodgeParameters().dodgeDistance;
	return Mathf::RotateVector(distance, player_->GetCamera()->quaternion_);
}

Vector3 PlayerStateDodge::CalculateFallbackDistance() const
{
	return Mathf::RotateVector(Vector3{ 0.0f, 0.0f, player_->GetDodgeParameters().dodgeDistance }, player_->GetDestinationQuaternion());
}

float PlayerStateDodge::CalculateEasingParameter() const
{
	return std::min<float>(1.0f, player_->GetAnimationTime() / player_->GetAnimationDuration());
}

Vector3 PlayerStateDodge::InterpolatePosition(float easingParameter) const
{
	return startPosition_ + (targetPosition_ - startPosition_) * Mathf::EaseInOutSine(easingParameter);
}

void PlayerStateDodge::FinalizeDodge()
{
	//アニメーション後の座標を修正
	Vector3 hipPosition = player_->GetHipWorldPosition();
	hipPosition.y = 0.0f;
	player_->SetPosition(hipPosition);

	//アニメーションを停止
	player_->StopAnimation();

	//通常状態に戻す
	player_->ChangeState(new PlayerStateRoot());
}