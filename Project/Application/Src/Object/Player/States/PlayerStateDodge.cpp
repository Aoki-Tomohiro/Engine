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

	//アニメーションの再生
	if (Mathf::Length(inputValue) > player_->GetRootParameters().walkThreshold)
	{
		//ロックオン中の場合
		if (player_->GetLockon()->ExistTarget())
		{
			//スティックの入力の絶対値を取得
			float horizontalValue = std::abs(inputValue.x);
			float verticalValue = std::abs(inputValue.z);

			//垂直方向の移動量が水平方向よりも大きい場合
			if (verticalValue >= horizontalValue)
			{
				if (inputValue.z > 0.0f)
				{
					//前進アニメーションを再生
					player_->PlayAnimation("Dodge1", 1.6f, false);
				}
				else
				{
					//後退アニメーションを再生
					player_->PlayAnimation("Dodge2", 1.6f, false);
				}
			}
			//水平方向の移動量が垂直方向よりも大きい場合
			else
			{
				if (inputValue.x < 0.0f)
				{
					//左移動のアニメーションを再生
					player_->PlayAnimation("Dodge3", 1.6f, false);
				}
				else
				{
					//右移動のアニメーションを再生
					player_->PlayAnimation("Dodge4", 1.6f, false);
				}
			}
		}
		else
		{
			//前進アニメーションを再生
			player_->PlayAnimation("Dodge1", 1.6f, false);
		}

		//移動距離を計算
		Vector3 distance = Mathf::Normalize(inputValue) * player_->GetDodgeParameters().dodgeDistance;

		//移動距離をカメラの角度だけ回転させる
		distance = Mathf::RotateVector(distance, player_->GetCamera()->quaternion_);

		//水平方向に移動させるので移動距離のY成分を0にする
		distance.y = 0.0f;

		//目標座標をカメラの角度だけ回転させる
		targetPosition_ = startPosition_ + distance;
	}
	else
	{
		//後退アニメーションを再生
		player_->PlayAnimation("Dodge2", 1.6f, false);

		//目標座標を計算
		targetPosition_ = startPosition_ + Mathf::RotateVector({ 0.0f,0.0f,player_->GetDodgeParameters().dodgeDistance }, player_->GetDestinationQuaternion());
	}
}

void PlayerStateDodge::Update()
{
	//イージングの係数を計算
	float easingParameter = std::min<float>(1.0f, player_->GetAnimationTime() / player_->GetAnimationDuration());

	//移動後の座標を計算
	Vector3 movedPosition = startPosition_ + (targetPosition_ - startPosition_) * Mathf::EaseInOutSine(easingParameter);

	//アニメーションによる座標のずれを修正
	player_->CorrectAnimationOffset();

	//プレイヤーに座標を設定
	player_->SetPosition(movedPosition);

	//アニメーションが終了した場合
	if (player_->GetIsAnimationFinished())
	{
		//アニメーション後の座標を代入
		Vector3 hipPosition = player_->GetHipWorldPosition();
		hipPosition.y = 0.0f;
		player_->SetPosition(hipPosition);

		//アニメーションを停止
		player_->StopAnimation();

		//通常状態に戻す
		player_->ChangeState(new PlayerStateRoot());
	}

	ImGui::Begin("Player");
	ImGui::DragFloat("EasingParameter", &easingParameter);
	ImGui::End();
}