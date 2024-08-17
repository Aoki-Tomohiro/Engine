#include "CameraStateDash.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Camera/States/CameraStateFollow.h"
#include "Application/Src/Object/Camera/States/CameraStateLockOn.h"
#include "Application/Src/Object/Player/Player.h"
#include <numbers>

void CameraStateDash::Initialize()
{
	//Quaternionの初期化
	const Player* player = GameObjectManager::GetInstance()->GetConstGameObject<Player>("");
	cameraController_->SetDestinationQuaternion(player->GetDestinationQuaternion() * Mathf::Conjugate(Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, std::numbers::pi_v<float>)));

	//Offset値の初期化
	cameraController_->SetDestinationOffset(cameraController_->GetDashCameraParameters().offset);

	//Fovの初期化
	startFov_ = cameraController_->GetCamera().fov_;
}

void CameraStateDash::Update()
{
	//追従対象からのオフセットを取得してカメラ座標を計算する
	Vector3 offset = cameraController_->Offset();
	cameraController_->SetPosition(cameraController_->GetInterTarget() + offset);

	//ロックオン中の場合は敵の方向に向かせる
	if (cameraController_->GetLockon()->ExistTarget())
	{
		//ロックオン対象の座標を取得
		Vector3 lockOnPosition = cameraController_->GetLockon()->GetTargetPosition();

		//差分ベクトルを計算
		Vector3 sub = Mathf::Normalize(lockOnPosition - cameraController_->GetPosition());

		//Quaternionの作成
		cameraController_->SetDestinationQuaternion(Mathf::LookRotation(sub, { 0.0f,1.0f,0.0f }));
	}

	//プレイヤーのダッシュのフラグが立っているとき
	const Player* player = GameObjectManager::GetInstance()->GetConstGameObject<Player>("");
	if (player->GetActionFlag(Player::ActionFlag::kDashing))
	{
		//Fovを線形補間
		easingTimer_ += GameTimer::GetDeltaTime();
		float easingParameter = std::clamp(easingTimer_ / cameraController_->GetDashCameraParameters().accelerationFovEasingDuration, 0.0f, 1.0f);
		cameraController_->SetFov(Mathf::Lerp(startFov_, cameraController_->GetDashCameraParameters().accelerationFov, easingParameter));

		//ロックオン中の場合はオフセット値を補間する
		if (cameraController_->GetLockon()->ExistTarget())
		{
			cameraController_->SetDestinationOffset(Mathf::Lerp(cameraController_->GetDashCameraParameters().offset, cameraController_->GetLockOnCameraParameters().offset, easingParameter));
		}
	}
	else
	{
		//Fovを線形補間
		easingTimer_ -= GameTimer::GetDeltaTime();
		float easingParameter = std::clamp(easingTimer_ / cameraController_->GetDashCameraParameters().decelerationFovEasingDuration, 0.0f, 1.0f);
		cameraController_->SetFov(Mathf::Lerp(startFov_, cameraController_->GetDashCameraParameters().accelerationFov, easingParameter));

		//線形補間が終了したら
		if (easingParameter <= 0.0f)
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
}