#include "PlayerStateJustDodge.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include <numbers>

void PlayerStateJustDodge::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションの設定
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.019");
	modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);

	//最初の座標を取得
	TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
	startPosition_ = transformComponent->GetWorldPosition();

	//スティックの入力を取得
	Vector3 stickValue = {
		input_->GetLeftStickX(),
		0.0f,
		input_->GetLeftStickY()
	};

	//スティックの入力が閾値を超えていた場合
	if (Mathf::Length(player_->velocity) > player_->walkThreshold_)
	{
		//速度を計算
		Vector3 offset = Mathf::Normalize(stickValue) * player_->justDodgeDistance_;

		//移動ベクトルをカメラの角度だけ回転させる
		Matrix4x4 rotateMatrix = Mathf::MakeRotateYMatrix(player_->camera_->rotation_.y);
		offset = Mathf::TransformNormal(offset, rotateMatrix);

		//目標座標を計算
		targetPosition_ = transformComponent->GetWorldPosition() + offset;
	}
	//スティックの入力がない場合
	else
	{
		//目標座標を計算
		targetPosition_ = transformComponent->GetWorldPosition() +Mathf::TransformNormal({ player_->justDodgeDistance_,0.0f,0.0f}, transformComponent->worldTransform_.matWorld_);
	}

	//進行方向に回転させる
	Vector3 sub = targetPosition_ - transformComponent->GetWorldPosition();
	Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, Mathf::Normalize(sub)));
	float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, Mathf::Normalize(sub));
	player_->destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
}

void PlayerStateJustDodge::Update()
{
	//移動処理
	easingParameter_ += 0.02f;
	TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
	transformComponent->worldTransform_.translation_.x = startPosition_.x + (targetPosition_.x - startPosition_.x) * Mathf::EaseOutExpo(easingParameter_);
	transformComponent->worldTransform_.translation_.y = startPosition_.y + (targetPosition_.y - startPosition_.y) * Mathf::EaseOutExpo(easingParameter_);
	transformComponent->worldTransform_.translation_.z = startPosition_.z + (targetPosition_.z - startPosition_.z) * Mathf::EaseOutExpo(easingParameter_);

	//イージングが終了したら通常状態に戻す
	if (easingParameter_ > 1.0f)
	{
		ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
		modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);
		player_->ChangeState(new PlayerStateRoot());
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