#include "PlayerStateRoot.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateJump.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateDodge.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateDash.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateAttack.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateMagicAttack.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateChargeMagicAttack.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateLaunchAttack.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateSpinAttack.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateStun.h"

void PlayerStateRoot::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションブレンドを有効化する
	player_->GetAnimator()->SetIsBlending(true);
}

void PlayerStateRoot::Update()
{
	//ゲームが終了していたら処理を飛ばす
	if (player_->GetIsGameFinished())
	{
		//現在のアニメーションがIdleではない場合再生する
		SetIdleAnimationIfNotPlaying();
		return;
	}

	//スティックの入力を取得
	Vector3 inputValue = {
		input_->GetLeftStickX(),
		0.0f,
		input_->GetLeftStickY(),
	};

	//入力ベクトルの長さを計算
	float inputLength = Mathf::Length(inputValue);

	//入力値が歩行の閾値を超えている場合
	if (inputLength > player_->GetRootParameters().walkThreshold)
	{
		//移動処理
		Move(inputValue, inputLength);
	}
	else
	{
		//現在のアニメーションがIdleではない場合再生する
		SetIdleAnimationIfNotPlaying();
	}

	//入力に基づいて状態を遷移
	ProcessStateTransition();
}

void PlayerStateRoot::OnCollision(GameObject* gameObject)
{
	//衝突処理
	player_->ProcessCollisionImpact(gameObject, true);
}

void PlayerStateRoot::SetIdleAnimationIfNotPlaying()
{
	//現在のアニメーションが通常状態ではない場合
	if (currentAnimationName_ != "Idle")
	{
		//現在のアニメーションを通常状態に変更
		currentAnimationName_ = "Idle";
		//歩きの閾値を超えていない場合は待機アニメーションを設定
		player_->GetAnimator()->PlayAnimation(currentAnimationName_, 1.0f, true);
	}
}

void PlayerStateRoot::Move(const Vector3& inputValue, const float inputLength)
{
	//入力値が走りの閾値を超えているかチェック
	bool isRunning = inputLength > player_->GetRootParameters().runThreshold;

	//アニメーションの更新
	UpdateAnimation(inputValue, isRunning);

	//移動速度を設定
	float moveSpeed = isRunning ? player_->GetRootParameters().runSpeed : player_->GetRootParameters().walkSpeed;
	//入力ベクトルを正規化し速度を掛ける
	Vector3 velocity = Mathf::Normalize(inputValue) * moveSpeed;

	//移動ベクトルにカメラの回転を適用
	velocity = Mathf::RotateVector(velocity, player_->GetCamera()->quaternion_);
	//水平方向に移動させるのでY成分を0にする
	velocity.y = 0.0f;

	//プレイヤーを移動させる
	player_->Move(velocity);

	//プレイヤーの回転を更新
	UpdateRotation(velocity);
}

void PlayerStateRoot::UpdateAnimation(const Vector3& inputValue, bool isRunning)
{
	//アニメーション名を格納するための変数
	std::string animationName;

	//ロックオン中のアニメーション決定
	if (player_->GetLockon()->ExistTarget())
	{
		//ロックオン中で、プレイヤーが走っているかどうかでアニメーションを決定
		if (isRunning)
		{
			//走行中の場合、入力ベクトルに基づいた走りアニメーションを選択
			animationName = DetermineRunningAnimation(inputValue);
		}
		else
		{
			//歩行中の場合、入力ベクトルに基づいた歩きアニメーションを選択
			animationName = DetermineWalkingAnimation(inputValue);
		}
	}
	else
	{
		//ロックオンしていない場合、走行または歩行に応じたデフォルトアニメーションを選択
		animationName = isRunning ? "Run1" : "Walk1";
	}

	//現在のアニメーションが変わっていた場合
	if (currentAnimationName_ != animationName)
	{
		//現在のアニメーションを更新
		currentAnimationName_ = animationName;
		player_->GetAnimator()->PlayAnimation(currentAnimationName_, 1.0f, true);
	}
}

const std::string PlayerStateRoot::DetermineWalkingAnimation(const Vector3& inputValue) const
{
	//入力ベクトルの方向に応じて歩行アニメーションを決定
	if (std::abs(inputValue.z) > std::abs(inputValue.x))
	{
		//前進または後退のアニメーション
		return (inputValue.z > 0.0f) ? "Walk1" : "Walk2";
	}
	else
	{
		//左右移動のアニメーション
		return (inputValue.x < 0.0f) ? "Walk3" : "Walk4";
	}
}

const std::string PlayerStateRoot::DetermineRunningAnimation(const Vector3& inputValue) const
{
	//入力ベクトルの方向に応じて走行アニメーションを決定
	if (std::abs(inputValue.z) > std::abs(inputValue.x))
	{
		//前進または後退のアニメーション
		return (inputValue.z > 0.0f) ? "Run1" : "Run2";
	}
	else
	{
		//左右移動のアニメーション
		return (inputValue.x < 0.0f) ? "Run3" : "Run4";
	}
}

void PlayerStateRoot::UpdateRotation(const Vector3& velocity)
{
	//ロックオン中の場合は敵の方向に向かせる
	if (player_->GetLockon()->ExistTarget())
	{
		player_->LookAtEnemy();
	}
	//ロックオンしていない場合は速度ベクトルの方向に回転
	else
	{
		player_->Rotate(Mathf::Normalize(velocity));
	}
}

void PlayerStateRoot::ProcessStateTransition()
{
	//RTの入力の閾値
	const float kRightTriggerThreshold = 0.7f;

	//Aボタンを押したとき
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
	{
		//ジャンプ状態に遷移
		player_->ChangeState(new PlayerStateJump());
		return;
	}
	//RBを押したとき
	else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_SHOULDER))
	{
		//回避状態に遷移
		player_->ChangeState(new PlayerStateDodge());
		return;
	}
	//Bボタンを押したとき
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
	{
		//ダッシュ状態に遷移
		player_->ChangeState(new PlayerStateDash());
		return;
	}

	//RTが押されていない場合
	if (input_->GetRightTriggerValue() < kRightTriggerThreshold)
	{
		//Xボタンを押したとき
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
		{
			//攻撃状態に遷移
			player_->ChangeState(new PlayerStateAttack());
		}
		//Yボタンを離した時
		else if (input_->IsPressButtonExit(XINPUT_GAMEPAD_Y))
		{
			//魔法攻撃状態に遷移
			if (player_->GetActionFlag(Player::ActionFlag::kMagicAttackEnabled))
			{
				player_->ChangeState(new PlayerStateMagicAttack());
			}
			//溜め魔法攻撃状態に遷移
			else if (player_->GetActionFlag(Player::ActionFlag::kChargeMagicAttackEnabled))
			{
				player_->ChangeState(new PlayerStateChargeMagicAttack());
			}
		}
	}
	else
	{
		//Xボタンを押したとき
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) && player_->GetIsCooldownComplete(Skill::kLaunchAttack) && player_->GetPosition().y == 0.0f)
		{
			//打ち上げ攻撃状態に遷移
			player_->ChangeState(new PlayerStateLaunchAttack());
		}
		//Yボタンを押したとき
		else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y) && player_->GetIsCooldownComplete(Skill::kSpinAttack))
		{
			//回転攻撃状態に遷移
			player_->ChangeState(new PlayerStateSpinAttack());
		}
	}
}