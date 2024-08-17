#include "PlayerStateJump.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Player/States/PlayerStateAttack.h"

void PlayerStateJump::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションの初期化
	player_->SetIsAnimationLoop(false);

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
		//走りジャンプのアニメーションを設定
		player_->SetAnimationName("Armature.001|mixamo.com|Layer0.013");

		//アニメーションの時間を設定
		player_->SetAnimationTime(0.0f);

		//走りの閾値を超えていた場合
		if (length > player_->GetRootParameters().runThreshold)
		{
			//速度を設定
			velocity_ = Mathf::Normalize(inputValue) * player_->GetRootParameters().runSpeed;
		}
		else
		{
			//速度を設定
			velocity_ = Mathf::Normalize(inputValue) * player_->GetRootParameters().walkSpeed;
		}

		//移動ベクトルをカメラの角度だけ回転させる
		velocity_ = Mathf::RotateVector(velocity_, player_->GetCamera()->quaternion_);

		//ジャンプの初速度を設定
		velocity_.y = player_->GetJumpParameters().jumpFirstSpeed;
	}
	else
	{
		//その場でジャンプするアニメーションを設定
		player_->SetAnimationName("Armature.001|mixamo.com|Layer0.012");

		//アニメーションの時間を設定
		player_->SetAnimationTime(0.1f);

		//ジャンプの初速度を設定
		velocity_.y = player_->GetJumpParameters().jumpFirstSpeed;
	}
}

void PlayerStateJump::Update()
{
	//重力加速度ベクトルの設定
	Vector3 accelerationVector = { 0.0f,player_->GetJumpParameters().gravityAcceleration,0.0f };

	//速度に重力加速度を加算
	velocity_ += accelerationVector;

	//移動処理
	player_->Move(velocity_);

	//地面についたら
	if (player_->GetPosition().y <= 0.0f)
	{
		//地面のめり込まないようにする
		Vector3 position = player_->GetPosition();
		position.y = 0.0f;
		player_->SetPosition(position);

		//通常状態に変更
		player_->ChangeState(new PlayerStateRoot());
	}
	//攻撃状態に遷移
	else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
	{
		player_->ChangeState(new PlayerStateAttack());
	}
}