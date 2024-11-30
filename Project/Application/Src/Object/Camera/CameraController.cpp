#include "CameraController.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Camera/States/CameraStateFollow.h"
#include "Application/Src/Object/Camera/States/CameraStateLockon.h"
#include "Application/Src/Object/Camera/States/CameraStateAnimation.h"

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
	//補間リセットの更新
	UpdateInterpolationReset();

	//追従座標を更新
	UpdateInterTargetPosition();

	//カメラの座標を更新
	UpdateCameraPosition();

	//状態の更新
	state_->Update();

	//カメラの回転を更新
	UpdateCameraRotation();

	//FOVを更新
	UpdateCameraFov();

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

void CameraController::StopAnimation()
{
	//補間リセット
	StartInterpolationReset(interpolationResetSettingDefaults_.easingType, interpolationResetSettingDefaults_.duration);

	//ロックオン中かどうかを判別して次の状態を決める
	lockon_->ExistTarget() ? ChangeState(new CameraStateLockon()) : ChangeState(new CameraStateFollow());
}

void CameraController::StartCameraShake(const Vector3& intensity, const float duration)
{
	//カメラシェイクを開始
	cameraShake_->Start(intensity, duration);
}

void CameraController::StartInterpolationReset(const CameraPath::EasingType easingType, const float duration)
{
	//補間リセットフラグを立てる
	interpolationResetSettings.isResetting = true;
	//イージングの種類を設定
	interpolationResetSettings.easingType = easingType;
	//補間速度リセットが完了するまでの時間
	interpolationResetSettings.duration = duration;
	//オフセットの補間速度を0にする
	interpolationSpeeds_.offset = 0.0f;
	//追従対象の補間速度を0にする
	interpolationSpeeds_.target = 0.0f;
	//回転の補間速度を0にする
	interpolationSpeeds_.quaternion = 0.0f;
	//FOVの補間速度を0にする
	interpolationSpeeds_.fov = 0.0f;
}

void CameraController::UpdateInterpolationReset()
{
	//補間リセットが無効なら処理を終了
	if (!interpolationResetSettings.isResetting) return;

	//タイマーを進める
	interpolationResetSettings.timer += GameTimer::GetDeltaTime();

	//イージング係数を計算
	float easingParameter = CalculateEasingParameter(interpolationResetSettings.timer, interpolationResetSettings.duration, interpolationResetSettings.easingType);

	//補間速度をリセット
	ResetInterpolationSpeeds(easingParameter);

	//リセットが完了したら状態を初期化
	if (interpolationResetSettings.timer >= interpolationResetSettings.duration)
	{
		interpolationResetSettings.isResetting = false;
		interpolationResetSettings.timer = 0.0f;
	}
}

void CameraController::ResetInterpolationSpeeds(const float easingParameter)
{
	//オフセットの補間速度をリセット
	interpolationSpeeds_.offset = Mathf::Lerp(0.0f, interpolationSpeedDefaults_.offset, easingParameter);
	//追従対象の補間速度をリセット
	interpolationSpeeds_.target = Mathf::Lerp(0.0f, interpolationSpeedDefaults_.target, easingParameter);
	//クォータニオンの補間速度をリセット
	interpolationSpeeds_.quaternion = Mathf::Lerp(0.0f, interpolationSpeedDefaults_.quaternion, easingParameter);
	//FOVの補間速度をリセット
	interpolationSpeeds_.fov = Mathf::Lerp(0.0f, interpolationSpeedDefaults_.fov, easingParameter);
}

void CameraController::UpdateInterTargetPosition()
{
	//追従対象が存在する場合は追従座標を補間する
	if (target_)
	{
		Vector3 targetPosition = target_->GetWorldPosition() + targetOffset_;
		interTarget_ = Mathf::Lerp(interTarget_, targetPosition, interpolationSpeeds_.target);
	}
}

void CameraController::UpdateCameraPosition()
{
	//オフセットを補間
	offset_ = Mathf::Lerp(offset_, destinationOffset_, interpolationSpeeds_.offset);
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
	camera_.quaternion_ = Mathf::Normalize(Mathf::Slerp(camera_.quaternion_, destinationQuaternion_, interpolationSpeeds_.quaternion));
}

void CameraController::UpdateCameraFov()
{
	//FOVを補間
	camera_.fov_ = Mathf::Lerp(camera_.fov_, destinationFov_, interpolationSpeeds_.fov);
}

float CameraController::CalculateEasingParameter(const float timer, const float duration, const CameraPath::EasingType easingType)
{
	//イージング係数を計算
	float easingParameter = timer / duration;
	easingParameter = std::clamp(easingParameter, 0.0f, 1.0f);

	//イージングタイプに応じた係数を計算
	switch (easingType)
	{
	case CameraPath::EasingType::kEaseIn:
		return Mathf::EaseInSine(easingParameter);
	case CameraPath::EasingType::kEaseOut:
		return Mathf::EaseOutSine(easingParameter);
	case CameraPath::EasingType::kEaseInOut:
		return Mathf::EaseInOutSine(easingParameter);
	default:
		return easingParameter;
	}
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