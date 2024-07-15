#include "PlayerStateIdle.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateWalk.h"
#include "Application/Src/Object/Player/States/PlayerStateRun.h"
#include "Application/Src/Object/Player/States/PlayerStateDodge.h"
#include "Application/Src/Object/Player/States/PlayerStateJustDodge.h"
#include "Application/Src/Object/Enemy/Enemy.h"

void PlayerStateIdle::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();
}

void PlayerStateIdle::Update()
{
	//アニメーションの設定
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	modelComponent->SetAnimationName("Armature|mixamo.com|Layer0");

	//スティックの入力を取得
	Vector3 value = {
		input_->GetLeftStickX(),
		0.0f,
		input_->GetLeftStickY()
	};

	//スティックの入力値を計算
	float length = Mathf::Length(value);

	//ジャスト回避の更新
	UpdateJustDodge();
	
	//RBで回避状態に遷移
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_SHOULDER) && !justDodgeSettings_.isJustDodgeSuccess)
	{
		player_->ChangeState(new PlayerStateDodge());
	}
	//ジャスト回避に成功していたらジャスト回避状態に遷移
	else if (justDodgeSettings_.isJustDodgeSuccess)
	{
		player_->ChangeState(new PlayerStateJustDodge());
	}
	//スティックの入力が走りの閾値より大きい場合、走り状態に遷移
	else if (length > player_->runThreshold_)
	{
		player_->ChangeState(new PlayerStateRun());
	}
	//スティックの入力が歩きの閾値を超えていた場合、歩き状態に遷移
	else if (length > player_->walkThreshold_)
	{
		player_->ChangeState(new PlayerStateWalk());
	}
}

void PlayerStateIdle::Draw(const Camera& camera)
{
}

void PlayerStateIdle::OnCollision(GameObject* other)
{
}

void PlayerStateIdle::OnCollisionEnter(GameObject* other)
{
}

void PlayerStateIdle::OnCollisionExit(GameObject* other)
{
}

void PlayerStateIdle::UpdateJustDodge()
{
	//敵の座標を取得
	Enemy* enemy = GameObjectManager::GetInstance()->GetGameObject<Enemy>();
	TransformComponent* enemyTransformComponent = enemy->GetComponent<TransformComponent>();
	//プレイヤーの座標を取得
	TransformComponent* playerTransformComponent = player_->GetComponent<TransformComponent>();

	//敵とプレイヤーの距離を計算
	Vector3 sub = playerTransformComponent->GetWorldPosition() - enemyTransformComponent->GetWorldPosition();

	//敵とプレイヤーの距離がジャスト回避が成功できる距離より近かった場合
	if (Mathf::Length(sub) < justDodgeSettings_.maxJustDodgeDistance)
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
		if (justDodgeSettings_.isJustDodgeAvailable && !justDodgeSettings_.isJustDodgeSuccess)
		{
			//ジャスト回避のタイマーを進める
			justDodgeSettings_.justDodgeTimer += GameTimer::GetDeltaTime();

			//ジャスト回避のタイマーが一定間隔を超えていない場合
			if (justDodgeSettings_.justDodgeTimer < justDodgeSettings_.justDodgeDuration)
			{
				//RBボタンを押したとき
				if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_SHOULDER))
				{
					//ジャスト回避を成功させる
					justDodgeSettings_.isJustDodgeSuccess = true;
				}
			}
		}
	}
}