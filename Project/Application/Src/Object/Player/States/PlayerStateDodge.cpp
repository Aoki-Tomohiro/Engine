#include "PlayerStateDodge.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Player/States/PlayerStateDead.h"

const std::array<PlayerStateDodge::DodgeDurations, PlayerStateDodge::DodgeDirection::NumDirections> PlayerStateDodge::kDodgeDurations_ =
{
	{
		{0.26f, 0.638f, 0.84f },
		{0.28f, 0.72f,  0.738f},
		{0.26f, 0.75f,  0.728f},
		{0.26f, 0.72f,  0.758f},
	}
};

void PlayerStateDodge::Initialize()
{
	//死亡状態に遷移
	if (player_->hp_ <= 0.0f)
	{
		player_->ChangeState(new PlayerStateDead());
		return;
	}

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
	if (Mathf::Length(player_->velocity) > player_->rootParameters_.walkThreshold)
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
		player_->velocity = Mathf::Normalize(player_->velocity) * player_->dodgeParameters_.dodgeSpeed;

		//移動ベクトルをカメラの角度だけ回転させる
		player_->velocity = Mathf::RotateVector(player_->velocity, player_->camera_->quaternion_);
		player_->velocity.y = 0.0f;
	}
	//スティックの入力がない場合は後ろに下がる
	else
	{
		//アニメーションの設定
		dodgeDirection_ = Backward;
		modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.011");

		//速度を計算
		TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
		player_->velocity = Mathf::TransformNormal({ 0.0f,0.0f,-player_->dodgeParameters_.dodgeSpeed }, transformComponent->worldTransform_.matWorld_);
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
	//溜め時間を超えていて、回避時間を超えていない場合
	if (animationTime > kDodgeDurations_[dodgeDirection_].chargeDuration && animationTime < kDodgeDurations_[dodgeDirection_].dodgeDuration)
	{
		//移動処理
		TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
		transformComponent->worldTransform_.translation_ += player_->velocity * GameTimer::GetDeltaTime();;
	}
	//アニメーションが終っていた場合
	else if (animationTime > kDodgeDurations_[dodgeDirection_].recoveryDuration)
	{
		//アニメーションをループ状態にする
		ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
		modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(1.0f);
		modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);
		//通常状態に遷移
		player_->ChangeState(new PlayerStateRoot());
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