#include "PlayerStateDodge.h"
#include "../Player.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GlobalVariables.h"

void PlayerStateDodge::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//速度が0でない場合
	if (player_->velocity != Vector3{ 0.0f,0.0f,0.0f })
	{
		//速度ベクトルを計算
		player_->velocity = Mathf::Normalize(player_->velocity) * dodgeSpeed_;

		//角度を設定
		rotationAngle_ = 0.2f;
	}
	//速度が0の場合
	else
	{
		//速度ベクトルを計算
		TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
		player_->velocity = Mathf::TransformNormal({ 0.0f,0.0f,-1.0f }, transformComponent->worldTransform_.matWorld_);

		//角度を設定
		rotationAngle_ = 0.4f;
	}

	//回転処理
	Vector3 axis = Mathf::Normalize(Mathf::Cross({ 0.0f,1.0f,0.0f }, Mathf::Normalize(player_->velocity)));
	Quaternion rotationQuaternion = Mathf::MakeRotateAxisAngleQuaternion(axis, rotationAngle_);
	player_->destinationQuaternion_ = rotationQuaternion * player_->destinationQuaternion_;

	//環境変数の設定
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "DodgeSpeed", dodgeSpeed_);
	globalVariables->AddItem(groupName, "DodgeTime", static_cast<int>(dodgeTime_));
}

void PlayerStateDodge::Update()
{
	//TransformComponentを取得
	TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();

	//速度を加算
	transformComponent->worldTransform_.translation_ += player_->velocity;

	//回避が終わったら
	if (++dodgeTimer_ > dodgeTime_)
	{
		//元の姿勢に戻す
		Vector3 axis = Mathf::Normalize(Mathf::Cross({ 0.0f,1.0f,0.0f }, Mathf::Normalize(player_->velocity)));
		Quaternion rotationQuaternion = Mathf::MakeRotateAxisAngleQuaternion(axis, -rotationAngle_);
		player_->destinationQuaternion_ = rotationQuaternion * player_->destinationQuaternion_;

		//通常状態に戻す
		player_->ChangeState(new PlayerStateRoot());
	}

	//環境変数の適用
	AppliGlobalVariables();
}

void PlayerStateDodge::Draw(const Camera& camera)
{
}

void PlayerStateDodge::AppliGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	dodgeSpeed_ = globalVariables->GetFloatValue(groupName, "DodgeSpeed");
	dodgeTime_ = globalVariables->GetIntValue(groupName, "DodgeTime");
}