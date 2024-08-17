#include "CameraStateFollow.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Camera/States/CameraStateLockOn.h"
#include "Application/Src/Object/Player/Player.h"
#include <numbers>

void CameraStateFollow::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//オフセット値の初期化
	cameraController_->SetDestinationOffset(cameraController_->GetFollowCameraParameters().offset);

	//Quaternionの初期化
	if (const Player* player = GameObjectManager::GetInstance()->GetConstGameObject<Player>(""))
	{
		quaternionY = player->GetDestinationQuaternion() * Mathf::Conjugate(Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, std::numbers::pi_v<float>));
		cameraController_->SetDestinationQuaternion(quaternionY);
	}
}

void CameraStateFollow::Update()
{
	//追従対象からのオフセットを取得してカメラ座標を計算する
	Vector3 offset = cameraController_->Offset();
	cameraController_->SetPosition(cameraController_->GetInterTarget() + offset);

	//スティックの入力のしきい値
	const float threshold = 0.7f;

	//右スティックの入力を取得
	Vector3 inputValue = {
		input_->GetRightStickY(),
		input_->GetRightStickX(),
		0.0f
	};

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

		//回転させないようにする
		const float offsetThreshold = -10.0f;
		if (destinationOffset.z > offsetThreshold)
		{
			if (inputValue.x >= 0.0f)
			{
				inputValue.x = 0.0f;
			}
		}
	}
	//カメラが地面に埋まっていない場合オフセット値をもとに戻す
	else
	{
		cameraController_->SetDestinationOffset(cameraController_->GetFollowCameraParameters().offset);
	}

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

		//Quaternionを設定
		cameraController_->SetDestinationQuaternion(Mathf::Normalize(rotationQuaternion));
	}

	//ロックオンカメラに遷移
	if (cameraController_->GetLockon()->ExistTarget())
	{
		cameraController_->ChangeState(new CameraStateLockOn());
	}
}

void CameraStateFollow::Reset()
{
	//追従対象がいれば
	if (cameraController_->GetTarget()) {
		//追従座標・角度の初期化
		Vector3 targetPosition = {
			cameraController_->GetTarget()->matWorld_.m[3][0],
			cameraController_->GetTarget()->matWorld_.m[3][1],
			cameraController_->GetTarget()->matWorld_.m[3][2],
		};
		cameraController_->SetInterTarget(targetPosition);
		cameraController_->SetDestinationQuaternion(cameraController_->GetTarget()->quaternion_);
	}

	//追従対象からのオフセット
	Vector3 offset = cameraController_->Offset();
	cameraController_->SetPosition(cameraController_->GetInterTarget() + offset);
}