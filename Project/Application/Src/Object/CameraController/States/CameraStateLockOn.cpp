#include "CameraStateLockOn.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/CameraController/CameraController.h"
#include "Application/Src/Object/CameraController/States/CameraStateFollow.h"
#include "Application/Src/Object/CameraController/States/CameraStateJustDodge.h"
#include "Application/Src/Object/Player/Player.h"

void CameraStateLockOn::Initialize()
{
	//オフセット値の初期化
	cameraController_->destinationOffset_ = { 2.0f, 2.0f, -24.0f };
}

void CameraStateLockOn::Update()
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

	//ロックオン対象の座標を取得
	Vector3 lockOnPosition = cameraController_->lockOn_->GetTargetPosition();

    //差分ベクトルを計算
	Vector3 sub = Mathf::Normalize(lockOnPosition - cameraController_->camera_.translation_);

    //Quaternionの作成
	cameraController_->destinationQuaternion_ = Mathf::LookRotation(sub, { 0.0f,1.0f,0.0f });

	//追従カメラに遷移
	if (!cameraController_->lockOn_->ExistTarget())
	{
		cameraController_->ChangeState(new CameraStateFollow());
	}
	//ジャスト回避のカメラに遷移
	else if (player->GetIsJustDodgeSuccess())
	{
		cameraController_->ChangeState(new CameraStateJustDodge());
	}
}