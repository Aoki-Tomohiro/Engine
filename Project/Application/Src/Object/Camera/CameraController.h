#pragma once
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Transform/WorldTransform.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Application/Src/Object/Lockon/LockOn.h"
#include "Application/Src/Object/Camera/CameraPathManager.h"
#include "Application/Src/Object/Camera/CameraShake.h"
#include "Application/Src/Object/Camera/States/ICameraState.h"
#include <numbers>

class CameraController
{
public:
	//通常カメラ時のパラメーター
	struct FollowCameraParameters
	{
		Vector3 offset = { 0.0f, 2.0f, -12.0f }; //オフセット値
		float rotationRangeMin = 1.6f;
		float rotationRangeMax = 2.4f;
		float rotationSpeedX = 0.04f;
		float rotationSpeedY = 0.06f;
	};

	//ロックオンカメラ時のパラメーター
	struct LockonCameraParameters
	{
		Vector3 offset = { 2.0f, 2.0f, -12.0f }; //オフセット値
		float rotationRangeMin = 1.0f;
		float rotationRangeMax = 1.8f;
	};

	void Initialize();

	void Update();

	void ChangeState(ICameraState* state);

	void StartCameraShake(const Vector3& intensity, const float duration);

	void StartInterpolationSpeedIncrease();

	const Vector3& GetInterTarget() const { return interTarget_; };

	const Vector3& GetPosition() const { return camera_.translation_; };

	void SetPosition(const Vector3& position) { camera_.translation_ = position; };

	const Vector3& GetDestinationOffset() const { return destinationOffset_; };

	void SetDestinationOffset(const Vector3& destinationOffset) { destinationOffset_ = destinationOffset; };

	const Quaternion& GetDestinationQuaternion() const { return destinationQuaternion_; };

	void SetDestinationQuaternion(const Quaternion& destinationQuaternion) { destinationQuaternion_ = destinationQuaternion; };

	const float GetFov() const { return camera_.fov_; };

	void SetFov(const float fov) { camera_.fov_ = fov; };

	const bool GetIsClearAnimationFinished() const { return isClearAnimationFinished_; };

	void SetIsClearAnimationFinished(const bool isClearAnimationFinished) { isClearAnimationFinished_ = isClearAnimationFinished; };

	CameraPathManager* GetCameraPathManager() const { return cameraPathManager_; };

	void SetCameraPathManager(CameraPathManager* cameraPathManager) { cameraPathManager_ = cameraPathManager; };

	const Lockon* GetLockon() const { return lockon_; };

	void SetLockon(const Lockon* lockOn) { lockon_ = lockOn; };

	void SetTarget(const WorldTransform* target) { target_ = target; };

	const Camera& GetCamera() const { return camera_; };

	const FollowCameraParameters& GetFollowCameraParameters() const { return followCameraParameters_; };

	const LockonCameraParameters& GetLockonCameraParameters() const { return lockonCameraParameters_; };

private:
	void UpdateInterTargetPosition();

	void UpdateCameraPosition();

	const Vector3 CalculateOffset() const;

	void UpdateAnimation();

	void UpdateEditing();

	void UpdateCameraTransform(const Quaternion& rotation, float fovDegrees);

	void UpdateInterpolationSpeed();

	void UpdateCameraOffset();

	void UpdateCameraRotation();

private:
	//カメラの状態
	std::unique_ptr<ICameraState> state_ = nullptr;

	//カメラシェイク
	std::unique_ptr<CameraShake> cameraShake_ = nullptr;

	//カメラ
	Camera camera_{};

	//カメラパスマネージャー
	CameraPathManager* cameraPathManager_ = nullptr;

	//ロックオン
	const Lockon* lockon_ = nullptr;

	//追従対象
	const WorldTransform* target_ = nullptr;

	//追従対象の残像座標
	Vector3 interTarget_{};

	//追従対象の補間の速度
	float targetInterpolationSpeed_ = 0.2f;

	//追従対象からのオフセット
	Vector3 offset_{ 0.0f, 4.0f, -20.0f };

	//補間用のオフセット
	Vector3 destinationOffset_{ 0.0f, 4.0f, -20.0f };

	//カメラのオフセット値の補間速度
	float offsetInterpolationSpeed_ = 0.1f;

	//Quaternion
	Quaternion destinationQuaternion_{ 0.0f,0.0f,0.0f,1.0f };

	//Quaternionの補間速度
	float quaternionInterpolationSpeed_ = 0.2f;

	//補間速度を徐々に増加させるフラグ
	bool isInterpolationSpeedIncreasing_ = false;

	//補間速度を増加させるためのタイマー
	float interpolationSpeedTimer_ = 0.0f;

	//補間が完了までの時間
	float remainingInterpolationTime_ = 0.2f;

	//クリアアニメーション状態かどうか
	bool isClearAnimationActive_ = false;

	//クリアアニメーションが終了したかどうか
	bool isClearAnimationFinished_ = false;

	//追従カメラ時のパラメーター
	FollowCameraParameters followCameraParameters_{};

	//ロックオンカメラ時のパラメーター
	LockonCameraParameters lockonCameraParameters_{};
};

