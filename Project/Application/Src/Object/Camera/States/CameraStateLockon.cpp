/**
 * @file CameraStateLockon.cpp
 * @brief ロックオンカメラの状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#include "CameraStateLockon.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Camera/States/CameraStateFollow.h"
#include "Application/Src/Object/Camera/States/CameraStateDebug.h"

void CameraStateLockon::Initialize()
{
	//オフセット値の初期化
	cameraController_->SetOffset(cameraController_->GetLockonCameraParameters().offset);

	//FOVの初期化
	cameraController_->SetFov(45.0f * (std::numbers::pi_v<float> / 180.0f));
}

void CameraStateLockon::Update()
{
	//ロックオン中の場合
	if (cameraController_->GetLockon()->ExistTarget())
	{
		//ロックオン対象の位置を取得
		Vector3 lockOnPosition = cameraController_->GetLockon()->GetTargetPosition();

		//カメラ座標を取得
		Vector3 cameraPosition = cameraController_->GetCameraPosition();

		//現在のカメラ位置からロックオン対象までの方向ベクトルを計算
		Vector3 normalizeDirection = Mathf::Normalize(lockOnPosition - cameraPosition);

		//Y軸のクォータニオンを計算
		Quaternion quaternionY = Mathf::Normalize(CalculateNewRotationY(normalizeDirection));

		//X軸のクォータニオンを計算
		Quaternion quaternionX = Mathf::Normalize(CalculateNewRotationX(normalizeDirection, quaternionY));

		//現在のクォータニオンに新しい回転を加算
		cameraController_->SetDestinationQuaternion(Mathf::Normalize(quaternionY * quaternionX));
	}

	//ロックオン対象がいない場合、追従カメラに切り替え
	if (!cameraController_->GetLockon()->ExistTarget())
	{
		cameraController_->ChangeState(new CameraStateFollow());
	}
	//デバッグのフラグが立っていた場合はデバッグ状態に遷移
	else if (cameraController_->GetCameraAnimationEditor()->GetIsDebug())
	{
		cameraController_->ChangeState(new CameraStateDebug());
	}
}

Quaternion CameraStateLockon::CalculateNewRotationY(const Vector3& directionVector) const
{
	//Y軸を0にしたベクトルを正規化
	Vector3 normalizedDirection = Mathf::Normalize(Vector3{ directionVector.x, 0.0f, directionVector.z });

	//回転軸を計算
	Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, normalizedDirection));

	//角度を計算
	float dot = std::clamp(Mathf::Dot({ 0.0f,0.0f,1.0f }, normalizedDirection), -1.0f, 1.0f);

	//ドット積から回転角度を計算
	float angle = std::acos(dot);

	//クロス積のY成分のドット積を計算して回転軸の方向を確認
	float direction = Mathf::Dot(cross, { 0.0f, 1.0f, 0.0f });

	//回転方向によってクォータニオンを生成
	return direction >= 0.0f
		? Mathf::MakeRotateAxisAngleQuaternion({ 0.0f, 1.0f, 0.0f }, angle)
		: Mathf::MakeRotateAxisAngleQuaternion({ 0.0f, 1.0f, 0.0f }, -angle);
}

Quaternion CameraStateLockon::CalculateNewRotationX(const Vector3& directionVector, const Quaternion& quaternionY) const
{
	//基準となる前方ベクトル
	Vector3 forward = Mathf::RotateVector({ 0.0f, 0.0f, 1.0f }, quaternionY);

	//クロス積を計算して回転方向を確認
	Vector3 cross = Mathf::Normalize(Mathf::Cross(forward, directionVector));

	//ドット積から角度を計算
	float dot = std::clamp(Mathf::Dot(forward, directionVector), -1.0f, 1.0f);

	//回転角度を計算
	float angle = std::min<float>(std::acos(dot), cameraController_->GetLockonCameraParameters().maxAngle);

	//クロス積のY成分のドット積を計算して回転方向を確認
	float direction = Mathf::Dot(cross, Mathf::RotateVector({ 1.0f, 0.0f, 0.0f }, quaternionY));

	//回転方向によってクォータニオンを生成
	return direction >= 0.0f
		? Mathf::MakeRotateAxisAngleQuaternion({ 1.0f, 0.0f, 0.0f }, angle)
		: Mathf::MakeRotateAxisAngleQuaternion({ 1.0f, 0.0f, 0.0f }, -angle);
}