#include "CameraStateBackhandAttack.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Camera/States/CameraStateFollow.h"
#include "Application/Src/Object/Camera/States/CameraStateLockOn.h"
#include "Application/Src/Object/Player/Player.h"

void CameraStateBackhandAttack::Initialize()
{
	cameraPath_ = cameraController_->GetCameraPathManager()->GetPath("BackhandAttack");
}

void CameraStateBackhandAttack::Update()
{
	//プレイヤーのアニメーションの時間を取得
	Player* player = GameObjectManager::GetInstance()->GetMutableGameObject<Player>("");
	float animationTime = player->GetAnimationTime();

	//アニメーションを進める
	CameraKeyFrame cameraKeyFrame = cameraPath_.GetInterpolatedKeyFrame(animationTime);

	//キーフレームの情報をもとに設定
	cameraController_->SetDestinationOffset(cameraKeyFrame.position);
	Quaternion quaternion = player->GetDestinationQuaternion() * Mathf::Conjugate(Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, std::numbers::pi_v<float>));
	cameraController_->SetDestinationQuaternion(quaternion * cameraKeyFrame.rotation);
	cameraController_->SetFov(cameraKeyFrame.fov * std::numbers::pi_v<float> / 180.0f);

	//追従対象からのオフセットを取得してカメラ座標を計算する
	Vector3 offset = cameraController_->Offset();
	cameraController_->SetPosition(cameraController_->GetInterTarget() + offset);

	//プレイヤーの落下攻撃が終了したら
	if (!player->GetActionFlag(Player::ActionFlag::kBackhandAttack))
	{
		//追従カメラに遷移
		if (!cameraController_->GetLockon()->ExistTarget())
		{
			cameraController_->ChangeState(new CameraStateFollow());
		}
		//ロックオンカメラに遷移
		else
		{
			cameraController_->ChangeState(new CameraStateLockOn());
		}
	}
}