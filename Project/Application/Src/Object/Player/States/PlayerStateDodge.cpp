#include "PlayerStateDodge.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"

void PlayerStateDodge::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションの初期化
	player_->SetIsAnimationLoop(false);
	player_->SetAnimationTime(0.0f);

	//スティックの入力を取得
	Vector3 inputValue = {
		input_->GetLeftStickX(),
		0.0f,
		input_->GetLeftStickY(),
	};
	
	//スティックの入力が歩きの閾値を超えていた場合
	if (Mathf::Length(inputValue) > player_->GetRootParameters().walkThreshold)
	{
		//アニメーションの設定
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
					player_->SetAnimationName("Armature.001|mixamo.com|Layer0.008"); //前進アニメーション

					//状態の取得
					state_ = player_->GetAnimationStateManager()->GetAnimationState("DodgeForward");

					//最初のフェーズの名前を設定
					phaseName_ = state_.phases[0].name;
				}
				else
				{
					player_->SetAnimationName("Armature.001|mixamo.com|Layer0.009"); //後退アニメーション

					//状態の取得
					state_ = player_->GetAnimationStateManager()->GetAnimationState("DodgeBackward");

					//最初のフェーズの名前を設定
					phaseName_ = state_.phases[0].name;
				}
			}
			//水平方向の移動量が垂直方向よりも大きい場合
			else
			{
				if (inputValue.x > 0.0f)
				{
					player_->SetAnimationName("Armature.001|mixamo.com|Layer0.011"); //右移動アニメーション

					//状態の取得
					state_ = player_->GetAnimationStateManager()->GetAnimationState("DodgeRight");

					//最初のフェーズの名前を設定
					phaseName_ = state_.phases[0].name;
				}
				else
				{
					player_->SetAnimationName("Armature.001|mixamo.com|Layer0.010"); //左移動アニメーション

					//状態の取得
					state_ = player_->GetAnimationStateManager()->GetAnimationState("DodgeLeft");

					//最初のフェーズの名前を設定
					phaseName_ = state_.phases[0].name;
				}
			}
		}
		else
		{
			player_->SetAnimationName("Armature.001|mixamo.com|Layer0.008"); //前進アニメーション

			//状態の取得
			state_ = player_->GetAnimationStateManager()->GetAnimationState("DodgeForward");

			//最初のフェーズの名前を設定
			phaseName_ = state_.phases[0].name;
		}

		//速度を計算
		velocity_ = Mathf::Normalize(inputValue) * player_->GetDodgeParameters().dodgeSpeed;

		//移動ベクトルをカメラの角度だけ回転させる
		velocity_ = Mathf::RotateVector(velocity_, player_->GetCamera()->quaternion_);

		//水平方向に移動させるので速度のY成分を0にする
		velocity_.y = 0.0f;
	}
	//スティックの入力がない場合は後ろに下がる
	else
	{
		//アニメーションの設定
		player_->SetAnimationName("Armature.001|mixamo.com|Layer0.009"); //後退アニメーション

		//状態の取得
		state_ = player_->GetAnimationStateManager()->GetAnimationState("DodgeBackward");

		//最初のフェーズの名前を設定
		phaseName_ = state_.phases[0].name;

		//速度を計算
		velocity_ = Mathf::RotateVector({ 0.0f,0.0f,player_->GetDodgeParameters().dodgeSpeed }, player_->GetDestinationQuaternion());
	}
}

void PlayerStateDodge::Update()
{
	//現在のアニメーションの時間を取得
	float animationTime = player_->GetAnimationTime();

	//状態の更新
	for (uint32_t i = 0; i < state_.phases.size() - 1; ++i)
	{
		if (phaseName_ == state_.phases[i].name)
		{
			if (animationTime >= state_.phases[i].animationTime)
			{
				phaseName_ = state_.phases[i + 1].name;
			}
		}
	}

	//フェーズごとの処理
	if (phaseName_ == "Dodge")
	{
		//移動処理
		player_->Move(velocity_);
	}

	//アニメーションが終了したら
	if (player_->GetIsAnimationEnd())
	{
		//アニメーション後の座標を代入
		Vector3 hipPosition = player_->GetHipWorldPosition();
		hipPosition.y = 0.0f;
		player_->SetPosition(hipPosition);

		//ロックオン中の場合敵の方向を向かせる
		if (player_->GetLockon()->ExistTarget())
		{
			player_->LookAtEnemy();
		}

		//通常状態に戻す
		player_->ChangeState(new PlayerStateRoot());
	}
}