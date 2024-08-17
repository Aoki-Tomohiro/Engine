#include "PlayerStateRoot.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateJump.h"
#include "Application/Src/Object/Player/States/PlayerStateDodge.h"
#include "Application/Src/Object/Player/States/PlayerStateDash.h"
#include "Application/Src/Object/Player/States/PlayerStateAttack.h"
#include "Application/Src/Object/Player/States/PlayerStateAttackBackhand.h"
#include "Application/Src/Object/Enemy/Enemy.h"

void PlayerStateRoot::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションの初期化
	player_->SetAnimationSpeed(1.0f);
	player_->SetIsAnimationLoop(true);
}

void PlayerStateRoot::Update()
{
	//スティックの入力を取得
	Vector3 velocity = {
		input_->GetLeftStickX(),
		0.0f,
		input_->GetLeftStickY(),
	};

	//スティックの入力が閾値を超えていた場合移動させる
	float length = Mathf::Length(velocity);
	if (length > player_->GetRootParameters().walkThreshold)
	{
		//スティックの入力が走りの閾値を超えていた場合
		if (length > player_->GetRootParameters().runThreshold)
		{
			//アニメーションの切り替え
			UpdateRunningAnimation(velocity);

			//速度を計算
			velocity = Mathf::Normalize(velocity) * player_->GetRootParameters().runSpeed;
		}
		else
		{
			//アニメーションの切り替え
			UpdateWalkingAnimation(velocity);

			//速度を計算
			velocity = Mathf::Normalize(velocity) * player_->GetRootParameters().walkSpeed;
		}

		//移動ベクトルをカメラの角度だけ回転させる
		velocity = Mathf::RotateVector(velocity, player_->GetCamera()->quaternion_);

		//水平方向に移動させるので速度のY成分を0にする
		velocity.y = 0.0f;

		//移動処理
		player_->Move(velocity);

		//ロックオン中の場合
		if (player_->GetLockon()->ExistTarget())
		{
			//敵の方向に回転させる
			player_->LookAtEnemy();
		}
		else
		{
			//速度ベクトルを適用
			Vector3 rotateVector = Mathf::Normalize(velocity);

			//回転処理
			player_->Rotate(Mathf::Normalize(rotateVector));
		}
	}
	//移動していなかった場合
	else
	{
		player_->SetAnimationName("Armature|mixamo.com|Layer0");
	}

	//LTを押している場合
	if (input_->GetLeftTriggerValue() < 0.7f)
	{
		//ジャンプ状態に遷移
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			player_->ChangeState(new PlayerStateJump());
		}
		//回避状態に遷移
		else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_SHOULDER))
		{
			player_->ChangeState(new PlayerStateDodge());
		}
		//ダッシュ状態に遷移
		else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
		{
			player_->ChangeState(new PlayerStateDash());
		}
		//攻撃状態に遷移
		else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
		{
			player_->ChangeState(new PlayerStateAttack());
		}
	}
	else
	{
		//バックハンド攻撃状態にする
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
		{
			player_->ChangeState(new PlayerStateAttackBackhand());
		}
	}
}

void PlayerStateRoot::UpdateWalkingAnimation(const Vector3& inputValue)
{
	if (player_->GetLockon()->ExistTarget())
	{
		//スティックの横入力の絶対値を取得
		float horizontalValue = std::abs(inputValue.x);

		//スティックの縦入力の絶対値を取得
		float verticalValue = std::abs(inputValue.z);

		//横入力値よりも縦入力値のほうが多い場合
		if (verticalValue > horizontalValue)
		{
			//前後の入力がある場合
			if (inputValue.z > 0.0f)
			{
				player_->SetAnimationName("Armature.001|mixamo.com|Layer0");
			}
			else
			{
				player_->SetAnimationName("Armature.001|mixamo.com|Layer0.001");
			}
		}
		else
		{
			//左右の入力がある場合
			if (inputValue.x < 0.0f)
			{
				player_->SetAnimationName("Armature.001|mixamo.com|Layer0.002");
			}
			else
			{
				player_->SetAnimationName("Armature.001|mixamo.com|Layer0.003");
			}
		}
	}
	else
	{
		player_->SetAnimationName("Armature.001|mixamo.com|Layer0");
	}
}

void PlayerStateRoot::UpdateRunningAnimation(const Vector3& inputValue)
{
	if (player_->GetLockon()->ExistTarget())
	{
		//スティックの横入力の絶対値を取得
		float horizontalValue = std::abs(inputValue.x);

		//スティックの縦入力の絶対値を取得
		float verticalValue = std::abs(inputValue.z);

		//横入力値よりも縦入力値のほうが多い場合
		if (verticalValue > horizontalValue)
		{
			//前後の入力がある場合
			if (inputValue.z > 0.0f)
			{
				player_->SetAnimationName("Armature.001|mixamo.com|Layer0.004");
			}
			else
			{
				player_->SetAnimationName("Armature.001|mixamo.com|Layer0.005");
			}
		}
		else
		{
			//左右の入力がある場合
			if (inputValue.x < 0.0f)
			{
				player_->SetAnimationName("Armature.001|mixamo.com|Layer0.006");
			}
			else
			{
				player_->SetAnimationName("Armature.001|mixamo.com|Layer0.007");
			}
		}
	}
	else
	{
		player_->SetAnimationName("Armature.001|mixamo.com|Layer0.004");
	}
}