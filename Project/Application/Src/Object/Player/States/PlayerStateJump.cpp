#include "PlayerStateJump.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Player/States/PlayerStateDash.h"

void PlayerStateJump::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションブレンドを無効化する
	player_->SetIsAnimationBlending(false);

	//スティックの入力を取得
	Vector3 inputValue = {
		input_->GetLeftStickX(),
		0.0f,
		input_->GetLeftStickY(),
	};

	//スティックの入力があれば
	float length = Mathf::Length(inputValue);
	if (length > player_->GetRootParameters().walkThreshold)
	{
		//入力値が走りの閾値を超えているかチェック
		bool isRunning = length > player_->GetRootParameters().runThreshold;

		//移動速度を設定
		float moveSpeed = isRunning ? player_->GetRootParameters().runSpeed : player_->GetRootParameters().walkSpeed;
		//入力ベクトルを正規化し速度を掛ける
		velocity_ = Mathf::Normalize(inputValue) * moveSpeed;

		//移動ベクトルにカメラの回転を適用
		velocity_ = Mathf::RotateVector(velocity_, player_->GetCamera()->quaternion_);

		//ダッシュジャンプのアニメーションを再生
		player_->PlayAnimation("Jump2", 1.0f, false);
	}
	else
	{
		//通常ジャンプのアニメーションを再生
		player_->PlayAnimation("Jump1", 1.0f, false);
	}

	//ジャンプの初速度を設定
	velocity_.y = player_->GetJumpParameters().jumpFirstSpeed;
}

void PlayerStateJump::Update()
{
	//重力加速度ベクトルの設定
	Vector3 accelerationVector = { 0.0f,player_->GetJumpParameters().gravityAcceleration,0.0f };

	//速度に重力加速度を加算
	velocity_ += accelerationVector;

	//移動処理
	player_->Move(velocity_);

	//プレイヤーの座標を取得
	Vector3 position = player_->GetPosition();

	//プレイヤーが地面についた場合
	if (position.y <= 0.0f)
	{
		//プレイヤーが地面に埋まらないように座標を補正
		position.y = 0.0f;
		player_->SetPosition(position);

		//通常状態に遷移
		player_->ChangeState(new PlayerStateRoot());
	}
	//Bボタンを押したとき
	else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
	{
		//ダッシュ状態に遷移
		player_->ChangeState(new PlayerStateDash());
	}
}