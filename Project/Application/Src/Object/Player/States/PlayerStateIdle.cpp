#include "PlayerStateIdle.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Application/Src/Object/Player/Player.h"
#include "PlayerStateJump.h"
#include "PlayerStateDodge.h"
#include "PlayerStateDash.h"
#include "PlayerStateGroundAttack.h"
#include "PlayerStateRangedAttack.h"

void PlayerStateIdle::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//環境変数の設定
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "MoveSpeed", moveSpeed_);
}

void PlayerStateIdle::Update()
{
	//移動フラグ
	bool isMove = false;

	//閾値
	const float kThreshold = 0.6f;

	//スティックの入力を取得
	player_->velocity = { 
		input_->GetLeftStickX(),
		0.0f,
		input_->GetLeftStickY()
	};

	//速度が閾値を超えていたら移動フラグをtrueにする
	if (Mathf::Length(player_->velocity) > kThreshold)
	{
		isMove = true;
	}

	//移動処理
	if (isMove)
	{
		//移動量に速さを反映
		player_->velocity = Mathf::Normalize(player_->velocity) * moveSpeed_;

		//移動ベクトルをカメラの角度だけ回転させる
		Matrix4x4 rotateMatrix = Mathf::MakeRotateYMatrix(player_->camera_->rotation_.y);
		player_->velocity = Mathf::TransformNormal(player_->velocity, rotateMatrix);

		//移動処理
		TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
		transformComponent->worldTransform_.translation_ += player_->velocity;

		//回転処理
		Vector3 vector = Mathf::Normalize(player_->velocity);
		Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, vector));
		float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, vector);
		player_->destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
	}
	//入力がない場合は速度を0にする
	else
	{
		player_->velocity = { 0.0f,0.0f,0.0f };
	}

	//ジャンプ状態に変更
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
	{
		player_->ChangeState(new PlayerStateJump());
	}
	//回避状態に変更
	else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_SHOULDER))
	{
		player_->ChangeState(new PlayerStateDodge());
	}
	//ダッシュ状態に変更
	else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
	{
		player_->ChangeState(new PlayerStateDash());
	}
	//地上攻撃の状態に遷移
	else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
	{
		player_->ChangeState(new PlayerStateGroundAttack());
	}
	//遠距離攻撃の状態に遷移
	else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y))
	{
		player_->ChangeState(new PlayerStateRangedAttack());
	}

	//環境変数の適用
	ApplyGlobalVariables();

	ImGui::Begin("PlayerStateRoot");
	ImGui::End();
}

void PlayerStateIdle::Draw(const Camera& camera)
{
}

void PlayerStateIdle::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	moveSpeed_ = globalVariables->GetFloatValue(groupName, "MoveSpeed");
}