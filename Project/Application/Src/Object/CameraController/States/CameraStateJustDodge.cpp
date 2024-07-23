#include "CameraStateJustDodge.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/CameraController/CameraController.h"
#include "Application/Src/Object/CameraController/States/CameraStateFollow.h"
#include "Application/Src/Object/CameraController/States/CameraStateLockOn.h"
#include "Application/Src/Object/Player/Player.h"

void CameraStateJustDodge::Initialize()
{
	//オフセット値を変更
	cameraController_->destinationOffset_ = { 3.0f,2.0f,-14.0f };
}

void CameraStateJustDodge::Update()
{
	//追従対象からのオフセットを取得してカメラ座標を計算する
	Vector3 offset = cameraController_->Offset();
	cameraController_->camera_.translation_ = cameraController_->interTarget_ + offset;
	if (cameraController_->camera_.translation_.y < 0.1f)
	{
		cameraController_->camera_.translation_.y = 0.1f;
	}

	//ロックオン対象の座標を取得
	Vector3 lockOnPosition = cameraController_->lockOn_->GetTargetPosition();

	//差分ベクトルを計算
	Vector3 direction = Mathf::Normalize(lockOnPosition - cameraController_->camera_.translation_);

	//Quaternionの作成
	cameraController_->destinationQuaternion_ = Mathf::LookRotation(direction, { 0.0f,1.0f,0.0f });

	//少し斜めさせる
	Quaternion quaternion = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,0.0f,1.0f }, 0.2f);
	cameraController_->destinationQuaternion_ = quaternion * cameraController_->destinationQuaternion_;

	//プレイヤーのジャスト回避が終了していたら
	Player* player = GameObjectManager::GetInstance()->GetGameObject<Player>();
	if (!player->GetIsJustDodgeSuccess())
	{
		//ロックオンカメラに遷移
		if (cameraController_->lockOn_->ExistTarget())
		{
			cameraController_->ChangeState(new CameraStateLockOn());
		}
		//追従カメラに遷移
		else
		{
			cameraController_->ChangeState(new CameraStateFollow());
		}
	}
}