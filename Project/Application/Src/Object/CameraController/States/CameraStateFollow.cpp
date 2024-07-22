#include "CameraStateFollow.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/CameraController/CameraController.h"
#include "Application/Src/Object/CameraController/States/CameraStateLockOn.h"
#include "Application/Src/Object/CameraController/States/CameraStateJustDodge.h"
#include "Application/Src/Object/Player/Player.h"
#include <numbers>

void CameraStateFollow::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//オフセット値の初期化
	cameraController_->destinationOffset_ = { 0.0f, 2.0f, -24.0f };

	//Quaternionの初期化
	if (cameraController_->target_)
	{
		//差分ベクトルを計算
		Vector3 direction = Mathf::Normalize(cameraController_->lockOn_->GetTargetPosition() - cameraController_->camera_.translation_);

		//Y軸成分を0にして水平面での方向ベクトルを得る
		direction.y = 0.0f;

		//回転軸を計算
		Vector3 cross = Mathf::Cross({ 0.0f,0.0f,1.0f }, direction);

		//角度を計算
		float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, direction);

		//Quaternionを作成
		quaternionY = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));

		//Quaternionを適用
		cameraController_->destinationQuaternion_ = quaternionY * quaternionX;
	}
}

void CameraStateFollow::Update()
{
	//プレイヤーが空中攻撃をしているときはOffset値を変える
	Player* player = GameObjectManager::GetInstance()->GetGameObject<Player>();
	if (player->GetIsAirAttack())
	{
		cameraController_->destinationOffset_ = { 0.0f, 2.0f, -24.0f };
	}
	else
	{
		cameraController_->destinationOffset_ = { 0.0f, 2.0f, -24.0f };
	}

	//追従対象からのオフセットを取得してカメラ座標を計算する
	Vector3 offset = cameraController_->Offset();
	cameraController_->camera_.translation_ = cameraController_->interTarget_ + offset;

	//スティックの入力のしきい値
	const float threshold = 0.7f;

	//右スティックの入力を取得
	Vector3 inputValue = {
		input_->GetRightStickY(),
		input_->GetRightStickX(),
		0.0f
	};

	//スティックの入力が遊び範囲を超えていた場合
	if (Mathf::Length(inputValue) > threshold)
	{
		//入力値に速度を掛ける
		Vector3 rotation = {
			inputValue.x * 0.02f,
			inputValue.y * 0.04f,
			0.0f,
		};

		//回転させるQuaternionの作成
		Quaternion newQuaternionX = Mathf::MakeRotateAxisAngleQuaternion({ 1.0f,0.0f,0.0f }, -rotation.x);
		Quaternion newQuaternionY = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, rotation.y);

		//回転を適用させる
		quaternionX = newQuaternionX * quaternionX;
		quaternionY = newQuaternionY * quaternionY;

		//Quaternionを合成
		Quaternion rotationQuaternion = quaternionY * quaternionX;

		//回転処理
		cameraController_->destinationQuaternion_ = Mathf::Normalize(rotationQuaternion);
	}

	//ロックオンカメラに遷移
	if (cameraController_->lockOn_->ExistTarget())
	{
		cameraController_->ChangeState(new CameraStateLockOn());
	}
	//ジャスト回避のカメラに遷移
	else if (player->GetIsJustDodgeSuccess())
	{
		cameraController_->ChangeState(new CameraStateJustDodge());
	}
}

void CameraStateFollow::Reset()
{
	//追従対象がいれば
	if (cameraController_->target_) {
		//追従座標・角度の初期化
		cameraController_->interTarget_ = cameraController_->target_->translation_;
		cameraController_->camera_.quaternion_ = cameraController_->target_->quaternion_;
	}
	cameraController_->destinationQuaternion_ = cameraController_->camera_.quaternion_;

	//追従対象からのオフセット
	Vector3 offset = cameraController_->Offset();
	cameraController_->camera_.translation_ = cameraController_->interTarget_ + offset;
}