#include "PlayerStateJump.h"
#include "../Player.h"
#include "PlayerStateIdle.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Utilities/GlobalVariables.h"

void PlayerStateJump::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//速度にジャンプの初速度を設定
	player_->velocity.y = jumpFirstSpeed_;

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
	transformComponent->worldTransform_.translation_ += player_->velocity;

	//地面に接触した場合の処理
	if (transformComponent->worldTransform_.translation_.y <= 1.0f)
	{
		//Root状態に変更
		player_->ChangeState(new PlayerStateIdle);

		//y座標を地面に固定
		transformComponent->worldTransform_.translation_.y = 1.0f;
	}

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