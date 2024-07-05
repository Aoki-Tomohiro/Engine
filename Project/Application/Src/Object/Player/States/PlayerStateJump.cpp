#include "PlayerStateJump.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/Player.h"
#include "PlayerStateIdle.h"
#include "PlayerStateAirAttack.h"
#include "PlayerStateRangedAttack.h"

void PlayerStateJump::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//速度にジャンプの初速度を設定
	player_->velocity.y = jumpFirstSpeed_;

	//アニメーションの初期化
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
	modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.003");

	//環境変数の設定
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "JumpFirstSpeed", jumpFirstSpeed_);
	globalVariables->AddItem(groupName, "GravityAcceleration", gravityAcceleration_);
}

void PlayerStateJump::Update()
{
	//TransformComponentを取得
	TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();

	//重力加速度ベクトルの設定
	Vector3 accelerationVector = { 0.0f,-gravityAcceleration_,0.0f };

	//速度に重力加速度を加算
	player_->velocity += accelerationVector;

	//速度を加算
	transformComponent->worldTransform_.translation_ += player_->velocity * GameTimer::GetDeltaTime();

	//通常状態に変更
	if (transformComponent->worldTransform_.translation_.y <= 0.0f)
	{
		transformComponent->worldTransform_.translation_.y = 0.0f;
		player_->ChangeState(new PlayerStateIdle);
	}
	////空中攻撃の状態に遷移
	//else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
	//{
	//	player_->ChangeState(new PlayerStateAirAttack());
	//}
	////遠距離攻撃の状態に遷移
	//else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y))
	//{
	//	player_->ChangeState(new PlayerStateRangedAttack());
	//}

	//環境変数の適用
	ApplyGlobalVariables();

	//ImGui
	ImGui::Begin("PlayerStateJump");
	ImGui::End();
}

void PlayerStateJump::Draw(const Camera& camera)
{
}


void PlayerStateJump::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	jumpFirstSpeed_ = globalVariables->GetFloatValue(groupName, "JumpFirstSpeed");
	gravityAcceleration_ = globalVariables->GetFloatValue(groupName, "GravityAcceleration");
}