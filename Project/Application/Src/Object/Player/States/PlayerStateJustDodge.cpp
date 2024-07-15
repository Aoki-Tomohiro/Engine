#include "PlayerStateJustDodge.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateIdle.h"
#include "Application/Src/Object/Player/States/PlayerStateWalk.h"
#include "Application/Src/Object/Player/States/PlayerStateRun.h"
#include <numbers>

void PlayerStateJustDodge::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションの設定
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	modelComponent->SetAnimationName("Armature|mixamo.com|Layer0");

	//スティックの入力を取得
	player_->velocity = {
		input_->GetLeftStickX(),
		0.0f,
		input_->GetLeftStickY()
	};

	//スティックの入力が歩きの閾値を超えていた場合
	if (Mathf::Length(player_->velocity) > player_->walkThreshold_)
	{
		//速度を計算
		player_->velocity = Mathf::Normalize(player_->velocity) * player_->justDodgeSpeed_;

		//移動ベクトルをカメラの角度だけ回転させる
		Matrix4x4 rotateMatrix = Mathf::MakeRotateYMatrix(player_->camera_->rotation_.y);
		player_->velocity = Mathf::TransformNormal(player_->velocity, rotateMatrix);
	}
	//スティックの入力がない場合
	else
	{
		//速度を計算
		TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
		player_->velocity = Mathf::TransformNormal({ 0.0f,0.0f,-player_->justDodgeSpeed_ }, transformComponent->worldTransform_.matWorld_);
	}
}

void PlayerStateJustDodge::Update()
{
	dodgeParameter_ += GameTimer::GetDeltaTime();
	if (dodgeParameter_ < justDodgeDuration_)
	{
		//移動処理
		TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
		transformComponent->worldTransform_.translation_ += player_->velocity * GameTimer::GetDeltaTime();

		//回転処理
		float fullRotationAngle = std::numbers::pi_v<float> * 2.0f;
		float rotationSpeed = fullRotationAngle / justDodgeDuration_;
		float rotationAngle = rotationSpeed * GameTimer::GetDeltaTime();
		Vector3 rotationAxis = { 0.0f, 1.0f, 0.0f };
		Quaternion quaternion = Mathf::MakeRotateAxisAngleQuaternion(rotationAxis, rotationAngle);
		player_->destinationQuaternion_ *= quaternion;
		transformComponent->worldTransform_.quaternion_ = player_->destinationQuaternion_;
	}

	if (dodgeParameter_ > recoveryDuration_)
	{
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

void PlayerStateJustDodge::Draw(const Camera& camera)
{
}

void PlayerStateJustDodge::OnCollision(GameObject* other)
{
}

void PlayerStateJustDodge::OnCollisionEnter(GameObject* other)
{
}

void PlayerStateJustDodge::OnCollisionExit(GameObject* other)
{
}