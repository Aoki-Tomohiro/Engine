#include "PlayerStateRoot.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateJump.h"
#include "Application/Src/Object/Player/States/PlayerStateDodge.h"
#include "Application/Src/Object/Player/States/PlayerStateDash.h"
#include "Application/Src/Object/Player/States/PlayerStateAttack.h"
#include "Application/Src/Object/Player/States/PlayerStateLaunchAttack.h"
#include "Application/Src/Object/Player/States/PlayerStateSpinAttack.h"
#include "Application/Src/Object/Player/States/PlayerStateStun.h"
#include "Application/Src/Object/Weapon/Weapon.h"

//現在のアニメーションの名前を保持するための変数
std::string PlayerStateRoot::currentAnimation_ = "Idle";

void PlayerStateRoot::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションブレンドを有効化する
	player_->SetIsAnimationBlending(true);

	//アニメーションを再生
	player_->PlayAnimation(currentAnimation_, 1.0f, true);
}

void PlayerStateRoot::Update()
{
	//ゲームが終了していたら処理を飛ばす
	if (player_->GetIsGameFinished())
	{
		player_->PlayAnimation("Idle", 1.0f, true);
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
		//現在のアニメーションが通常状態ではない場合
		if (currentAnimation_ != "Idle")
		{
			//現在のアニメーションを通常状態に変更
			currentAnimation_ = "Idle";
			//歩きの閾値を超えていない場合は待機アニメーションを設定
			player_->PlayAnimation("Idle", 1.0f, true);
		}
	}

	//入力に基づいて状態を遷移
	ProcessStateTransition();
}

void PlayerStateRoot::OnCollision(GameObject* other)
{
	//衝突相手が武器だった場合
	if (Weapon* weapon = dynamic_cast<Weapon*>(other))
	{
		//ダメージを食らった処理を実行
		player_->HandleIncomingDamage(weapon, true);
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
	if (currentAnimation_ != animationName)
	{
		//現在のアニメーションを更新
		currentAnimation_ = animationName;
		player_->PlayAnimation(currentAnimation_, 1.0f, true);
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

void PlayerStateRoot::ProcessStateTransition()
{
	//RTの入力の閾値
	const float kRightTriggerThreshold = 0.7f;

	//RTが押されていない場合
	if (input_->GetRightTriggerValue() < kRightTriggerThreshold)
	{
		//Aボタンを押したとき
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			//ジャンプ状態に遷移
			player_->ChangeState(new PlayerStateJump());
		}
		//RBを押したとき
		else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_SHOULDER))
		{
			//回避状態に遷移
			player_->ChangeState(new PlayerStateDodge());
		}
		//Bボタンを押したとき
		else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
		{
			//ダッシュ状態に遷移
			player_->ChangeState(new PlayerStateDash());
		}
		//Xボタンを押したとき
		else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
		{
			//攻撃状態に遷移
			player_->ChangeState(new PlayerStateAttack());
		}
	}
	else
	{
		//Xボタンを押したとき
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) && player_->GetIsCooldownComplete(Skill::kLaunchAttack))
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