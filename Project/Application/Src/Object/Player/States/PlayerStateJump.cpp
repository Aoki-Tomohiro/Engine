#include "PlayerStateJump.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"

void PlayerStateJump::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションの初期化
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
	if (player_->velocity == Vector3{ 0.0f,0.0f,0.0f } || player_->lockOn_->ExistTarget())
	{
		modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.008");
		modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.18f);
	}
	else
	{
		modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.009");
	}

	//速度にジャンプの初速度を設定
	player_->velocity.y = player_->jumpParameters_.firstSpeed;
}

void PlayerStateJump::Update()
{
	//TransformComponentを取得
	TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();

	//重力加速度ベクトルの設定
	Vector3 accelerationVector = { 0.0f,-player_->jumpParameters_.gravityAcceleration,0.0f };

	//速度に重力加速度を加算
	player_->velocity += accelerationVector;

	//速度を加算
	transformComponent->worldTransform_.translation_ += player_->velocity * GameTimer::GetDeltaTime();

	//通常状態に変更
	if (transformComponent->worldTransform_.translation_.y <= 0.0f)
	{
		//プレイヤーを地面の座標に設定
		transformComponent->worldTransform_.translation_.y = 0.0f;

		//速度の初期化
		player_->velocity.y = 0.0f;

		//通常状態に遷移
		player_->ChangeState(new PlayerStateRoot());
	}
}

void PlayerStateJump::Draw(const Camera& camera)
{
}

void PlayerStateJump::OnCollision(GameObject* other)
{
}

void PlayerStateJump::OnCollisionEnter(GameObject* other)
{
}

void PlayerStateJump::OnCollisionExit(GameObject* other)
{
}