#include "CameraStateLockOn.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Camera/States/CameraStateFollow.h"
#include "Application/Src/Object/Camera/States/CameraStateDash.h"
#include "Application/Src/Object/Camera/States/CameraStateFallingAttack.h"
#include "Application/Src/Object/Camera/States/CameraStateBackhandAttack.h"
#include "Application/Src/Object/Player/Player.h"

void CameraStateLockOn::Initialize()
{
	//オフセット値の初期化
	cameraController_->SetDestinationOffset(cameraController_->GetLockOnCameraParameters().offset);
}

void CameraStateLockOn::Update()
{
	//追従対象からのオフセットを取得してカメラ座標を計算する
	Vector3 offset = cameraController_->Offset();
	cameraController_->SetPosition(cameraController_->GetInterTarget() + offset);

	//カメラが地面に埋まっている場合カメラの制限を付ける
	const float kMinHeight = 0.0f;
	if (cameraController_->GetPosition().y <= kMinHeight)
	{
		//オフセット値をカメラがめり込んだ量だけ引く
		Vector3 destinationOffset = cameraController_->GetDestinationOffset();
		destinationOffset.z -= cameraController_->GetPosition().y;
		cameraController_->SetDestinationOffset(destinationOffset);
		cameraController_->SetOffset(destinationOffset);

		//オフセット値を再計算
		offset = cameraController_->Offset();
		cameraController_->SetPosition(cameraController_->GetInterTarget() + offset);
	}
	//カメラが地面に埋まっていない場合オフセット値をもとに戻す
	else
	{
		cameraController_->SetDestinationOffset(cameraController_->GetLockOnCameraParameters().offset);
	}


	//ロックオン対象の座標を取得
	Vector3 lockOnPosition = cameraController_->GetLockon()->GetTargetPosition();

	//差分ベクトルを計算
	Vector3 sub = Mathf::Normalize(lockOnPosition - cameraController_->GetPosition());

	//X軸の角度を計算
	float dot = std::acos(Mathf::Dot({ 0.0f,1.0f,0.0f }, { 0.0f,sub.y,0.0f }));

	//X軸の角度が一定範囲内の場合は回転させる
	if (dot > std::numbers::pi_v<float> / 2.0f && dot < std::numbers::pi_v<float> - std::numbers::pi_v<float> / 4.0f)
	{
		cameraController_->SetDestinationQuaternion(Mathf::LootAt(cameraController_->GetPosition(), lockOnPosition));
	}
	
	//プレイヤーを取得
	const Player* player = GameObjectManager::GetInstance()->GetConstGameObject<Player>("");
	//追従カメラに遷移
	if (!cameraController_->GetLockon()->ExistTarget())
	{
		cameraController_->ChangeState(new CameraStateFollow());
	}
	//ダッシュカメラに遷移
	else if (player->GetActionFlag(Player::ActionFlag::kDashing))
	{
		cameraController_->ChangeState(new CameraStateDash());
	}
	//落下攻撃カメラに遷移
	else if (player->GetActionFlag(Player::ActionFlag::kFallingAttack))
	{
		cameraController_->ChangeState(new CameraStateFallingAttack());
	}
	//バックハンド攻撃カメラに遷移
	else if (player->GetActionFlag(Player::ActionFlag::kBackhandAttack))
	{
		cameraController_->ChangeState(new CameraStateBackhandAttack());
	}
}