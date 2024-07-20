#include "PlayerStateRoot.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateDodge.h"
#include "Application/Src/Object/Player/States/PlayerStateJustDodge.h"
#include "Application/Src/Object/Player/States/PlayerStateJump.h"
#include "Application/Src/Object/Player/States/PlayerStateDash.h"
#include "Application/Src/Object/Player/States/PlayerStateGroundAttack.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Warning/Warning.h"

void PlayerStateRoot::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();
}

void PlayerStateRoot::Update()
{
	//スティックの入力の更新
	UpdateStickInput();

	//スティックの入力が閾値を超えていた場合
	if (inputLength_ > player_->walkThreshold_)
	{
		//移動処理
		UpdateMovement();

		//回転処理
		UpdateRotation();

		//ロックオン中のアニメーションの更新
		UpdateLockOnAnimation();
	}
	//スティックの入力が閾値を超えていない場合
	else
	{
		//待機アニメーションの更新
		UpdateIdleAnimation();

		//速度をゼロにする
		player_->velocity = { 0.0f,0.0f,0.0f };
	}

	//ジャスト回避の処理
	UpdateJustDodge();

	//警告範囲に入っているかのフラグのリセット
	justDodgeSettings_.isInWarningRange = false;

	//回避状態に遷移
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_SHOULDER) && !player_->isJustDodgeSuccess_)
	{
		player_->ChangeState(new PlayerStateDodge());
	}
	//ジャスト回避状態に遷移
	else if (player_->isJustDodgeSuccess_)
	{
		player_->ChangeState(new PlayerStateJustDodge());
	}
	//ジャンプ状態に遷移
	else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
	{
		player_->ChangeState(new PlayerStateJump());
	}
	//ダッシュ状態の遷移
	else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
	{
		player_->ChangeState(new PlayerStateDash());
	}
	//攻撃状態に遷移
	else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
	{
		player_->ChangeState(new PlayerStateGroundAttack());
	}
}

void PlayerStateRoot::Draw(const Camera& camera)
{
}

void PlayerStateRoot::OnCollision(GameObject* other)
{
	//衝突相手が警告オブジェクトだった場合
	if (dynamic_cast<Warning*>(other))
	{
		justDodgeSettings_.isInWarningRange = true;
	}
}

void PlayerStateRoot::OnCollisionEnter(GameObject* other)
{
}

void PlayerStateRoot::OnCollisionExit(GameObject* other)
{
}

void PlayerStateRoot::UpdateStickInput()
{
	//スティックの入力を取得
	inputValue_ = {
		input_->GetLeftStickX(),
		0.0f,
		input_->GetLeftStickY()
	};

	//スティックの入力が歩きの閾値を超えていた場合
	inputLength_ = Mathf::Length(inputValue_);
}

void PlayerStateRoot::UpdateMovement()
{
	//スティックの入力が走りの閾値を超えていた場合
	if (inputLength_ > player_->runThreshold_)
	{
		//移動量に速さを反映
		player_->velocity = Mathf::Normalize(inputValue_) * player_->runSpeed_;

		//走り状態にする
		isRunning_ = true;
	}
	else
	{
		//移動量に速さを反映
		player_->velocity = Mathf::Normalize(inputValue_) * player_->walkThreshold_;

		//歩き状態にする
		isRunning_ = false;
	}

	//移動ベクトルをカメラの角度だけ回転させる
	player_->velocity = Mathf::RotateVector(player_->velocity, player_->camera_->quaternion_);
	player_->velocity.y = 0.0f;

	//移動処理
	TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
	transformComponent->worldTransform_.translation_ += player_->velocity * GameTimer::GetDeltaTime();
}

void PlayerStateRoot::UpdateRotation()
{
	//回転ベクトル
	Vector3 rotateVector{};

	//ロックオン中の場合は敵の方向に向ける
	if (player_->lockOn_->ExistTarget())
	{
		//差分ベクトルを計算
		TransformComponent* playerTransformComponent = player_->GetComponent<TransformComponent>();
		TransformComponent* enemyTransformConponent = GameObjectManager::GetInstance()->GetGameObject<Enemy>()->GetComponent<TransformComponent>();
		rotateVector = Mathf::Normalize(enemyTransformConponent->GetWorldPosition() - playerTransformComponent->GetWorldPosition());
		rotateVector.y = 0.0f;
	}
	//ロックオン中ではない場合は進行方向に向ける
	else
	{
		rotateVector = Mathf::Normalize(player_->velocity);
	}

	//Quaternionの計算
	Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, rotateVector));
	float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, rotateVector);
	player_->destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
}

void PlayerStateRoot::UpdateIdleAnimation()
{
	//待機アニメーションに変える
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	modelComponent->SetAnimationName("Armature|mixamo.com|Layer0");
}

void PlayerStateRoot::UpdateLockOnAnimation()
{
	//ロックオン中の場合はアニメーションを切り替える
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	if (player_->lockOn_->ExistTarget())
	{
		//スティックの横入力の絶対値を取得
		float horizontalValue = std::abs(inputValue_.x);

		//スティックの縦入力の絶対値を取得
		float verticalValue = std::abs(inputValue_.z);

		//横入力値よりも縦入力値のほうが多い場合
		if (verticalValue > horizontalValue)
		{
			//前後の入力がある場合
			if (inputValue_.z > 0.0f)
			{
				if (isRunning_)
				{
					modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.004"); //前進アニメーション
				}
				else
				{
					modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0"); //前進アニメーション
				}
			}
			else
			{
				if (isRunning_)
				{
					modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.005"); //後退アニメーション
				}
				else
				{
					modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.001"); //後退アニメーション
				}
			}
		}
		else
		{
			//左右の入力がある場合
			if (inputValue_.x > 0.0f)
			{
				if (isRunning_)
				{
					modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.006"); //右横移動アニメーション
				}
				else
				{
					modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.003"); //右横移動アニメーション
				}
			}
			else
			{
				if (isRunning_)
				{
					modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.007"); //左横移動アニメーション
				}
				else
				{
					modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.002"); //左横移動アニメーション
				}
			}
		}
	}
	else
	{
		if (isRunning_)
		{
			//前進アニメーション
			modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.004");
		}
		else
		{
			//前進アニメーション
			modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0");
		}
	}
}

void PlayerStateRoot::UpdateJustDodge()
{
	//敵の座標を取得
	Enemy* enemy = GameObjectManager::GetInstance()->GetGameObject<Enemy>();
	TransformComponent* enemyTransformComponent = enemy->GetComponent<TransformComponent>();
	//プレイヤーの座標を取得
	TransformComponent* playerTransformComponent = player_->GetComponent<TransformComponent>();

	//敵とプレイヤーの距離を計算
	Vector3 sub = playerTransformComponent->GetWorldPosition() - enemyTransformComponent->GetWorldPosition();

	//敵の攻撃範囲に入っている場合
	if (justDodgeSettings_.isInWarningRange)
	{
		//敵とプレイヤーの距離がジャスト回避が成功できる距離より近かった場合
		if (Mathf::Length(sub) < player_->justDodgeDistance_)
		{
			//敵の攻撃が終了したらフラグをリセット
			if (!enemy->GetIsWarning())
			{
				justDodgeSettings_.isJustDodgeAvailable = false;
			}

			//ジャスト回避が可能ではない場合
			if (!justDodgeSettings_.isJustDodgeAvailable)
			{
				//敵が攻撃した場合
				if (enemy->GetIsWarning())
				{
					//ジャスト回避が可能にする
					justDodgeSettings_.isJustDodgeAvailable = true;
				}
			}

			//ジャスト回避が可能でジャスト回避が成功していない場合
			if (justDodgeSettings_.isJustDodgeAvailable && !player_->isJustDodgeSuccess_)
			{
				//ジャスト回避のタイマーを進める
				justDodgeSettings_.justDodgeTimer += GameTimer::GetDeltaTime();

				//ジャスト回避のタイマーが一定間隔を超えていない場合
				if (justDodgeSettings_.justDodgeTimer < player_->justDodgeDuration_)
				{
					//RBボタンを押したとき
					if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_SHOULDER))
					{
						//ジャスト回避を成功させる
						player_->isJustDodgeSuccess_ = true;
					}
				}
			}
		}
	}
}