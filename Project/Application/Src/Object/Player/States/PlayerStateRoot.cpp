#include "PlayerStateRoot.h"
#include "../Player.h"
#include "PlayerStateJump.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Math/MathFunction.h"

void PlayerStateRoot::Initialize()
{
	input_ = Input::GetInstance();
}

void PlayerStateRoot::Update()
{
	//移動フラグ
	bool isMove = false;

	//移動速度
	const float kMoveSpeed = 0.2f;

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
		player_->velocity = Mathf::Normalize(player_->velocity) * kMoveSpeed;

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

	//ジャンプ状態に変更
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
	{
		player_->ChangeState(new PlayerStateJump());
	}

	ImGui::Begin("PlayerStateRoot");
	ImGui::End();
}

void PlayerStateRoot::Draw(const Camera& camera)
{
}