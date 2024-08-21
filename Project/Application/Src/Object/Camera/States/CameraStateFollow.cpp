#include "CameraStateFollow.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Camera/States/CameraStateLockon.h"
#include "Application/Src/Object/Player/Player.h"

void CameraStateFollow::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//オフセット値の初期化
	cameraController_->SetDestinationOffset(cameraController_->GetFollowCameraParameters().offset);

	//Quaternionの初期化
	if (const Player* player = GameObjectManager::GetInstance()->GetConstGameObject<Player>(""))
	{
		quaternionY = player->GetDestinationQuaternion() * Mathf::Conjugate(Mathf::MakeRotateAxisAngleQuaternion({ 0.0f, 1.0f, 0.0f }, std::numbers::pi_v<float>));
		cameraController_->SetDestinationQuaternion(quaternionY);
	}
}

void CameraStateFollow::Update()
{
	//カメラの回転を更新
	UpdateCameraRotation();

	//ロックオンカメラに遷移
	if (cameraController_->GetLockon()->ExistTarget())
	{
		cameraController_->ChangeState(new CameraStateLockon());
	}
}

void CameraStateFollow::UpdateCameraRotation()
{
	//スティック入力によるカメラ回転処理のしきい値
	const float threshold = 0.7f;

	//右スティックの入力値を取得
	Vector3 inputValue = GetRightStickInput();

	//入力値の大きさがしきい値を超えている場合のみ処理
	if (Mathf::Length(inputValue) > threshold)
	{
		//回転量を計算
		Vector3 rotation = {
			inputValue.x * 0.02f,
			inputValue.y * 0.04f,
			0.0f,
		};

		//X軸回転のクォータニオンを作成
		Quaternion newQuaternionX = Mathf::MakeRotateAxisAngleQuaternion({ 1.0f, 0.0f, 0.0f }, -rotation.x);

		//Y軸回転のクォータニオンを作成
		Quaternion newQuaternionY = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f, 1.0f, 0.0f }, rotation.y);

		//現在のクォータニオンに新しい回転を適用
		quaternionX = newQuaternionX * quaternionX;
		quaternionY = newQuaternionY * quaternionY;

		//X軸とY軸の回転クォータニオンを合成
		Quaternion rotationQuaternion = quaternionY * quaternionX;

		//合成された回転クォータニオンを正規化してカメラの目的クォータニオンに設定
		cameraController_->SetDestinationQuaternion(Mathf::Normalize(rotationQuaternion));
	}
}

Vector3 CameraStateFollow::GetRightStickInput() const
{
	return {
		input_->GetRightStickY(),
		input_->GetRightStickX(),
		0.0f
	};
}