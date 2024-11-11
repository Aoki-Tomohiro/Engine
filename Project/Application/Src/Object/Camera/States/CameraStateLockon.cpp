#include "CameraStateLockon.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Camera/States/CameraStateFollow.h"

void CameraStateLockon::Initialize()
{
	//オフセット値の初期化
	cameraController_->SetOffset(cameraController_->GetLockonCameraParameters().offset);

	//ロックオン対象の位置を取得
	Vector3 lockOnPosition = cameraController_->GetLockon()->GetTargetPosition();

	//現在のカメラ位置からロックオン対象までの方向ベクトルを計算
	currentDirection_ = Mathf::Normalize(lockOnPosition - cameraController_->GetCameraPosition());

	//前のフレームの方向ベクトルを更新
	previousDirection_ = currentDirection_;
}

void CameraStateLockon::Update()
{
	//前のフレームの方向ベクトルを更新
	previousDirection_ = currentDirection_;

	//ロックオン対象の位置を取得
	Vector3 lockOnPosition = cameraController_->GetLockon()->GetTargetPosition();

	//カメラ座標を取得
	Vector3 cameraPosition = cameraController_->GetCameraPosition();

	//現在のカメラ位置からロックオン対象までの方向ベクトルを計算
	currentDirection_ = Mathf::Normalize(lockOnPosition - cameraPosition);

	//プレイヤーと敵が一定距離中にいる場合
	if (IsCameraCloseToTarget(cameraPosition, lockOnPosition))
	{
		//クォータニオンによる回転を計算
		Quaternion newQuaternion = CalculateNewRotation();
		//現在のクォータニオンに新しい回転を加算
		cameraController_->SetDestinationQuaternion(Mathf::Normalize(newQuaternion * cameraController_->GetDestinationQuaternion()));
	}
	//プレイヤーと敵が一定距離外であり前の方向と現在の方向が異なる場合
	else if (previousDirection_ != currentDirection_)
	{
		cameraController_->SetDestinationQuaternion(Mathf::LookAt(cameraPosition, lockOnPosition));
	}

	//ロックオン対象がいない場合、追従カメラに切り替え
	if (!cameraController_->GetLockon()->ExistTarget())
	{
		cameraController_->ChangeState(new CameraStateFollow());
	}
}

const Quaternion CameraStateLockon::CalculateNewRotation() const
{
	//前の方向ベクトルと現在の方向ベクトルを正規化して水平面上の方向を取得
	Vector3 previous = Mathf::Normalize(Vector3{ previousDirection_.x, 0.0f, previousDirection_.z });
	Vector3 current = Mathf::Normalize(Vector3{ currentDirection_.x, 0.0f, currentDirection_.z });

	//正規化した前の方向ベクトルと現在の方向ベクトルのドット積を計算
	float dotProduct = Mathf::Dot(previous, current);

	//ドット積が-1から1の範囲に収まるように制限
	dotProduct = std::clamp(dotProduct, -1.0f, 1.0f);

	//ドット積から回転角度を計算
	float angle = std::acos(dotProduct);

	//前の方向ベクトルと現在の方向ベクトルのクロス積を計算
	Vector3 cross = Mathf::Cross(previous, current);

	//クロス積のY成分のドット積を計算して回転軸の方向を確認
	float direction = Mathf::Dot(cross, { 0.0f, 1.0f, 0.0f });

	//回転方向によってクォータニオンを生成
	return direction >= 0.0f
		? Mathf::MakeRotateAxisAngleQuaternion({ 0.0f, 1.0f, 0.0f }, angle)
		: Mathf::MakeRotateAxisAngleQuaternion({ 0.0f, 1.0f, 0.0f }, -angle);
}

const bool CameraStateLockon::IsCameraCloseToTarget(const Vector3& cameraPosition, const Vector3& lockOnTargetPosition) const
{
	return Mathf::Length(lockOnTargetPosition - cameraPosition) <= cameraController_->GetLockonCameraParameters().maxDistance;
}