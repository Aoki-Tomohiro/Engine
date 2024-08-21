#include "PlayerStateRoot.h"
#include "Application/Src/Object/Player/Player.h"

void PlayerStateRoot::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションの初期化
	player_->SetIsAnimationLoop(true);
	player_->SetAnimationSpeed(1.0f);
}

void PlayerStateRoot::Update()
{
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
		//歩きの閾値を超えていない場合は待機アニメーションを設定
		player_->SetAnimationName("Armature|mixamo.com|Layer0");
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
		animationName = isRunning ? "Armature.001|mixamo.com|Layer0.004" : "Armature.001|mixamo.com|Layer0";
	}

	//決定したアニメーションをプレイヤーに設定
	player_->SetAnimationName(animationName);
}

const std::string PlayerStateRoot::DetermineWalkingAnimation(const Vector3& inputValue) const
{
	//入力ベクトルの方向に応じて歩行アニメーションを決定
	if (std::abs(inputValue.z) > std::abs(inputValue.x))
	{
		//前進または後退のアニメーション
		return (inputValue.z > 0.0f) ? "Armature.001|mixamo.com|Layer0" : "Armature.001|mixamo.com|Layer0.001";
	}
	else
	{
		//左右移動のアニメーション
		return (inputValue.x < 0.0f) ? "Armature.001|mixamo.com|Layer0.002" : "Armature.001|mixamo.com|Layer0.003";
	}
}

const std::string PlayerStateRoot::DetermineRunningAnimation(const Vector3& inputValue) const
{
	//入力ベクトルの方向に応じて走行アニメーションを決定
	if (std::abs(inputValue.z) > std::abs(inputValue.x))
	{
		//前進または後退のアニメーション
		return (inputValue.z > 0.0f) ? "Armature.001|mixamo.com|Layer0.004" : "Armature.001|mixamo.com|Layer0.005";
	}
	else
	{
		//左右移動のアニメーション
		return (inputValue.x < 0.0f) ? "Armature.001|mixamo.com|Layer0.006" : "Armature.001|mixamo.com|Layer0.007";
	}
}