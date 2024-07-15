#include "PlayerStateDodge.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateIdle.h"
#include "Application/Src/Object/Player/States/PlayerStateWalk.h"
#include "Application/Src/Object/Player/States/PlayerStateRun.h"

void PlayerStateDodge::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	// アニメーションの初期化
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f); //アニメーションの再生時間を初期化
	modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(1.2f);
	modelComponent->GetModel()->GetAnimation()->SetIsLoop(false); //アニメーションをループしないように設定

	//スティックの入力を取得
	player_->velocity = {
		input_->GetLeftStickX(),
		0.0f,
		input_->GetLeftStickY()
	};

	//スティックの入力が歩きの閾値を超えていた場合
	if (Mathf::Length(player_->velocity) > player_->walkThreshold_)
	{
		//アニメーションの設定
		if (player_->lockOn_->ExistTarget())
		{
			//速度の絶対値を取得
			float horizontalValue = std::abs(player_->velocity.x);
			float verticalValue = std::abs(player_->velocity.z);

			//垂直方向の移動量が水平方向よりも大きい場合
			if (verticalValue >= horizontalValue)
			{
				if (player_->velocity.z > 0.0f)
				{
					dodgeDirection_ = Forward;
					modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.010"); //前進アニメーション
				}
				else
				{
					dodgeDirection_ = Backward;
					modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.011"); //後退アニメーション
				}
			}
			//水平方向の移動量が垂直方向よりも大きい場合
			else
			{
				if (player_->velocity.x > 0.0f)
				{
					dodgeDirection_ = Right;
					modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.013"); //右横移動アニメーション
				}
				else
				{
					dodgeDirection_ = Left;
					modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.012"); //左横移動アニメーション
				}
			}
		}
		else
		{
			dodgeDirection_ = Forward;
			modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.010"); //前進アニメーション
		}

		//速度を計算
		player_->velocity = Mathf::Normalize(player_->velocity) * player_->dodgeSpeed_;

		//移動ベクトルをカメラの角度だけ回転させる
		Matrix4x4 rotateMatrix = Mathf::MakeRotateYMatrix(player_->camera_->rotation_.y);
		player_->velocity = Mathf::TransformNormal(player_->velocity, rotateMatrix);
	}
	//スティックの入力がない場合は後ろに下がる
	else
	{
		//アニメーションの設定
		dodgeDirection_ = Backward;
		modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.011");

		//速度を計算
		TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
		player_->velocity = Mathf::TransformNormal({ 0.0f,0.0f,-player_->dodgeSpeed_ }, transformComponent->worldTransform_.matWorld_);
	}
}

void PlayerStateDodge::Update()
{
	//デバッグのフラグが立っているときは処理を飛ばす
	if (player_->isDebug_)
	{
		return;
	}

	//ModelConponentを取得
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	float animationTime = modelComponent->GetModel()->GetAnimation()->GetAnimationTime();
	if (animationTime > dodgeSettings_[dodgeDirection_].chargeDuration && animationTime < dodgeSettings_[dodgeDirection_].dodgeDuration)
	{
		//移動処理
		TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
		transformComponent->worldTransform_.translation_ += player_->velocity * GameTimer::GetDeltaTime();;
	}

	//アニメーションが終っていた場合
	if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
	{
		//アニメーションをループ状態にする
		ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
		modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(1.0f);
		modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);

		//スティックの入力を取得
		Vector3 value = {
			input_->GetLeftStickX(),
			0.0f,
			input_->GetLeftStickY()
		};

		//入力の値に応じて状態を遷移
		float length = Mathf::Length(value);
		if (length > player_->runThreshold_)
		{
			player_->ChangeState(new PlayerStateRun());
		}
		else if (length > player_->walkThreshold_)
		{
			player_->ChangeState(new PlayerStateWalk());
		}
		else
		{
			player_->ChangeState(new PlayerStateIdle());
		}
	}
}

void PlayerStateDodge::Draw(const Camera& camera)
{
}

void PlayerStateDodge::OnCollision(GameObject* other)
{
}

void PlayerStateDodge::OnCollisionEnter(GameObject* other)
{
}

void PlayerStateDodge::OnCollisionExit(GameObject* other)
{
}