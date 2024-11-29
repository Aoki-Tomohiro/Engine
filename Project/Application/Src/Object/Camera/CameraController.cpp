#include "CameraController.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Camera/States/CameraStateFollow.h"
#include "Application/Src/Object/Camera/States/CameraStateAnimation.h"
#include <numbers>

void CameraController::Initialize()
{
	//カメラの初期化
	camera_.Initialize();
	camera_.rotationType_ = RotationType::Quaternion;

	//カメラシェイクの生成
	cameraShake_ = std::make_unique<CameraShake>();

	//状態の初期化
	ChangeState(new CameraStateFollow());
}

void CameraController::Update()
{
	//リセットのフラグが立っていた場合
	if (resetInterpolationSpeedGradually_)
	{
		//タイマーを進める
		resetInterpolationSpeedGraduallyTimer_ += GameTimer::GetDeltaTime();

		//イージング係数を計算
		float easingParameter = resetInterpolationSpeedGraduallyTimer_ / resetInterpolationSpeedGraduallyTime_;

		//イージングの種類に応じた処理
		switch (easingType_)
		{
		case CameraPath::EasingType::kEaseIn:
			easingParameter = Mathf::EaseInSine(easingParameter);
			break;
		case CameraPath::EasingType::kEaseOut:
			easingParameter = Mathf::EaseOutSine(easingParameter);
			break;
		case CameraPath::EasingType::kEaseInOut:
			easingParameter = Mathf::EaseInOutSine(easingParameter);
			break;
		}

		//イージングが1.0を超えたない様にする
		easingParameter = std::min<float>(1.0f, easingParameter);

		//補間速度を徐々に戻す
		offsetInterpolationSpeed_ = Mathf::Lerp(0.0f, 0.6f, easingParameter);
		targetInterpolationSpeed_ = Mathf::Lerp(0.0f, 0.2f, easingParameter);
		quaternionInterpolationSpeed_ = Mathf::Lerp(0.0f, 0.4f, easingParameter);
		camera_.fov_ = Mathf::Lerp(currentFov_, 45.0f * (std::numbers::pi_v<float> / 180.0f), easingParameter);

		//補間が終了していたらリセットのフラグをリセット
		if (resetInterpolationSpeedGraduallyTimer_ >= resetInterpolationSpeedGraduallyTime_)
		{
			resetInterpolationSpeedGradually_ = false;
			resetInterpolationSpeedGraduallyTimer_ = 0.0f;
		}
	}

	//追従座標を更新
	UpdateInterTargetPosition();

	//カメラの座標を更新
	UpdateCameraPosition();

	//状態の更新
	state_->Update();

	//カメラの回転を更新
	UpdateCameraRotation();

	//カメラシェイクの処理
	cameraShake_->Update();
	camera_.translation_ += cameraShake_->GetShakeOffset();

	//カメラの更新
	camera_.UpdateMatrix();
}

void CameraController::ChangeState(ICameraState* state)
{
	//新しいカメラの状態を設定
	state->SetCameraController(this);
	//新しいカメラの状態を初期化
	state->Initialize();
	//現在のカメラ状態を新しい状態に置き換える
	state_.reset(state);
}

void CameraController::PlayAnimation(const std::string& animationName, const float animationSpeed, const bool syncWithCharacterAnimation)
{
	//アニメーションの状態に遷移
	ChangeState(new CameraStateAnimation(animationName, animationSpeed, syncWithCharacterAnimation));
}

void CameraController::StartCameraShake(const Vector3& intensity, const float duration)
{
	//カメラシェイクを開始
	cameraShake_->Start(intensity, duration);
}

void CameraController::Reset(const CameraPath::EasingType easingType, const float resetInterpolationSpeedGraduallyTime)
{
	resetInterpolationSpeedGradually_ = true;
	easingType_ = easingType;
	resetInterpolationSpeedGraduallyTime_ = resetInterpolationSpeedGraduallyTime;
	offsetInterpolationSpeed_ = 0.0f;
	targetInterpolationSpeed_ = 0.0f;
	quaternionInterpolationSpeed_ = 0.0f;
	currentFov_ = camera_.fov_;
}

void CameraController::UpdateInterTargetPosition()
{
	//追従対象が存在する場合は追従座標を補間する
	if (target_)
	{
		Vector3 targetPosition = target_->GetWorldPosition() + targetOffset_;
		interTarget_ = Mathf::Lerp(interTarget_, targetPosition, targetInterpolationSpeed_);
	}
}

void CameraController::UpdateCameraPosition()
{
	//オフセットを補間
	offset_ = Mathf::Lerp(offset_, destinationOffset_, offsetInterpolationSpeed_);
	//追従対象からオフセットを計算
	Vector3 offset = CalculateOffset();
	//カメラの目標座標（通常位置）
	Vector3 desiredPosition = interTarget_ + offset;
	//地面と衝突している場合は交差点を座標に代入
	camera_.translation_ = CheckRayIntersectsGround(interTarget_, desiredPosition - interTarget_, Mathf::Length(offset_)) ? intersectionPoint_ : desiredPosition;
}

void CameraController::UpdateCameraRotation()
{
	//目標クォータニオンを正規化
	destinationQuaternion_ = Mathf::Normalize(destinationQuaternion_);
	//現在のクォータニオン補間
	camera_.quaternion_ = Mathf::Normalize(Mathf::Slerp(camera_.quaternion_, destinationQuaternion_, quaternionInterpolationSpeed_));
}

const Vector3 CameraController::CalculateOffset() const
{
	//追従対象からのオフセットを取得
	Vector3 offset = offset_;
	//カメラの回転に基づいてオフセットベクトルを回転させる
	offset = Mathf::RotateVector(offset, camera_.quaternion_);
	//回転を適用したオフセット値を返す
	return offset;
}

const bool CameraController::CheckRayIntersectsGround(const Vector3& rayOrigin, const Vector3& rayDirection, float maxDistance)
{
	//地面の点
	const Vector3 planePoint = { 0.0f, 0.0f, 0.0f };
	//平面の法線
	const Vector3 planeNormal = { 0.0f, 1.0f, 0.0f };

	//Rayの方向と平面の法線ベクトルの内積を計算
	float denominator = Mathf::Dot(planeNormal, Mathf::Normalize(rayDirection));

	//Rayが平面と平行である場合は交差しない
	if (std::fabs(denominator) < std::numeric_limits<float>::epsilon())
	{
		return false;
	}

	//平面上の任意の点とRayの始点とのベクトルを計算
	Vector3 originToPlane = planePoint - rayOrigin;
	float t = Mathf::Dot(originToPlane, planeNormal) / denominator;

	//衝突距離が正でかつmaxDistance以内の場合に衝突と判定
	if (t >= 0 && t <= maxDistance)
	{
		intersectionPoint_ = rayOrigin + Mathf::Normalize(rayDirection) * t;
		return true;
	}

	return false;
}