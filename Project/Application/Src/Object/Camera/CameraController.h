#pragma once
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Transform/WorldTransform.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Application/Src/Object/Lockon/LockOn.h"
#include "Application/Src/Object/Camera/CameraPathManager.h"
#include "Application/Src/Object/Camera/States/ICameraState.h"
#include <numbers>

class CameraController
{
public:
	//通常カメラ時のパラメーター
	struct FollowCameraParameters
	{
		Vector3 offset = { 0.0f, 2.0f, -10.0f }; //オフセット値
	};

	//ロックオンカメラ時のパラメーター
	struct LockonCameraParameters
	{
		Vector3 offset = { 2.0f, 2.0f, -10.0f }; //オフセット値
		float rotationRangeMin = std::numbers::pi_v<float> / 4.0f;
		float rotationRangeMax = std::numbers::pi_v<float> * 3.0f / 4.0f;
	};

	void Initialize();

	void Update();

	void ChangeState(ICameraState* state);

	void UpdateCameraPosition();

	const Vector3 CalculateOffset() const;

	const Vector3& GetInterTarget() const { return interTarget_; };

	const Vector3& GetPosition() const { return camera_.translation_; };

	void SetPosition(const Vector3& position) { camera_.translation_ = position; };

	const Vector3& GetDestinationOffset() const { return destinationOffset_; };

	void SetDestinationOffset(const Vector3& destinationOffset) { destinationOffset_ = destinationOffset; };

	const Quaternion& GetDestinationQuaternion() const { return destinationQuaternion_; };

	void SetDestinationQuaternion(const Quaternion& destinationQuaternion) { destinationQuaternion_ = destinationQuaternion; };

	void SetCameraPathManager(CameraPathManager* cameraPathManager) { cameraPathManager_ = cameraPathManager; };

	const Lockon* GetLockon() const { return lockon_; };

	void SetLockon(const Lockon* lockOn) { lockon_ = lockOn; };

	void SetTarget(const WorldTransform* target) { target_ = target; };

	const Camera& GetCamera() const { return camera_; };

	const FollowCameraParameters& GetFollowCameraParameters() const { return followCameraParameters_; };

	const LockonCameraParameters& GetLockonCameraParameters() const { return lockonCameraParameters_; };

private:
	void UpdateInterTargetPosition();

	void UpdateAnimation();

	void UpdateEditing();

	void UpdateCameraTransform(const Quaternion& rotation, float fovDegrees);

	void UpdateCameraOffset();

	void UpdateCameraRotation();

private:
	//カメラの状態
	std::unique_ptr<ICameraState> state_ = nullptr;

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

	//追従カメラ時のパラメーター
	FollowCameraParameters followCameraParameters_{};

	//ロックオンカメラ時のパラメーター
	LockonCameraParameters lockonCameraParameters_{};
};

