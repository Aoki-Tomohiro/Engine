#pragma once
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Transform/WorldTransform.h"
#include "Engine/Utilities/RandomGenerator.h"
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
	struct LockOnCameraParameters
	{
		Vector3 offset = { 2.0f, 2.0f, -10.0f }; //オフセット値
	};

	//ダッシュカメラ時のパラメーター
	struct DashCameraParameters
	{
		Vector3 offset = { 0.0f, 2.0f, -10.0f };                            //オフセット値
		float accelerationFov = 70.0f * std::numbers::pi_v<float> / 180.0f; //加速時のFov
		float accelerationFovEasingDuration = 0.2f;                         //加速時の線形補間が終了するまでの時間
		float decelerationFovEasingDuration = 1.0f;                         //通常状態に戻す線形補間が終了するまでの時間
	};

	void Initialize();

	void Update();

	void ChangeState(ICameraState* state);

	const Vector3 Offset() const;

	const Vector3& GetPosition() const { return camera_.translation_; };

	void SetPosition(const Vector3& position) { camera_.translation_ = position; };

	const Vector3& GetDestinationOffset() const { return destinationOffset_; };

	void SetDestinationOffset(const Vector3& destinationOffset) { destinationOffset_ = destinationOffset; };

	const Vector3& GetOffset() const { return offset_; };

	void SetOffset(const Vector3& offset) { offset_ = offset; };

	const Quaternion& GetDestinationQuaternion() const { return destinationQuaternion_; };

	void SetDestinationQuaternion(const Quaternion& destinationQuaternion) { destinationQuaternion_ = destinationQuaternion; };

	const Vector3& GetInterTarget() const { return interTarget_; };

	void SetInterTarget(const Vector3& interTarget) { interTarget_ = interTarget; };

	const WorldTransform* GetTarget() const { return target_; };

	void SetTarget(const WorldTransform* target) { target_ = target; };

	const float GetFov() const { return camera_.fov_; };

	void SetFov(const float fov) { camera_.fov_ = fov; };

	CameraPathManager* GetCameraPathManager() const { return cameraPathManager_; };

	void SetCameraPathManager(CameraPathManager* cameraPathManager) { cameraPathManager_ = cameraPathManager; };

	const Lockon* GetLockon() const { return lockon_; };

	void SetLockon(const Lockon* lockOn) { lockon_ = lockOn; };

	const Camera& GetCamera() const { return camera_; };

	void SetCamera(const Camera& camera) { camera_ = camera; };

	const FollowCameraParameters& GetFollowCameraParameters() const { return followCameraParameters_; };

	const LockOnCameraParameters& GetLockOnCameraParameters() const { return lockOnCameraParameters_; };

	const DashCameraParameters& GetDashCameraParameters() const { return dashCameraParameters_; };

private:
	//カメラシェイク用の構造体
	struct CameraShakeSettings
	{
		bool isShaking = false;                             // カメラシェイクが有効かどうかのフラグ
		Vector3 intensity = { 0.0f, 0.6f, 0.0f };           // カメラシェイクの強度
		Vector3 originalPosition{};                         // カメラシェイクの基準座標オフセット
		float duration = 0.2f;                              // カメラシェイクの持続時間
		float timer = 0.0f;                                 // カメラシェイクの経過時間を計測するタイマー
	};

	//カメラシェイク用のパラメーター
	struct CameraShakeParameter
	{
		Vector3 baseIntensity = { 0.0f, 0.6f, 0.0f };          // カメラシェイクの強度
		Vector3 justDodgeIntensity = { 0.0f, 4.0f, 0.0f };     // 回避攻撃のカメラシェイクの強度
		Vector3 enhancedMagicIntensity = { 0.0f, 0.8f, 0.0f }; // 強化魔法のカメラシェイクの強度
		Vector3 chargedMagicIntensity = { 0.0f, 6.0f, 0.0f };  // チャージ魔法のカメラシェイクの強度
		Vector3 damagedIntensity = { 0.0f, 3.0f, 0.0f };       // 被弾時のカメラシェイクの強度
		float baseDuration = 0.2f;                             // カメラシェイクの持続時間
		float justDodgeDuration = 0.6f;                        // 回避攻撃のカメラシェイクの持続時間
		float enhancedMagicDuration = 0.4f;                    // 強化魔法のカメラシェイクの持続時間
		float chargedMagicDuration = 0.6f;                     // 強化魔法のカメラシェイクの持続時間
		float damagedDuration = 0.8f;                          // 被弾時のカメラシェイクの持続時間
	};

	void UpdateCameraShake();

private:
	//カメラの状態
	std::unique_ptr<ICameraState> state_ = nullptr;

	//カメラ
	Camera camera_{};

	//CameraPathManager
	CameraPathManager* cameraPathManager_ = nullptr;

	//ロックオン
	const Lockon* lockon_ = nullptr;

	//追従対象
	const WorldTransform* target_ = nullptr;

	//追従対象の残像座標
	Vector3 interTarget_{};

	//追従対象からのオフセット
	Vector3 offset_{ 0.0f, 4.0f, -20.0f };

	//補間用のオフセット
	Vector3 destinationOffset_{ 0.0f, 4.0f, -20.0f };

	//Quaternion
	Quaternion destinationQuaternion_{ 0.0f,0.0f,0.0f,1.0f };

	//追従対象の補間の速度
	float targetInterpolationSpeed_ = 0.2f;

	//カメラのオフセット値の補間速度
	float offsetInterpolationSpeed_ = 0.1f;

	//Quaternionの補間速度
	float quaternionInterpolationSpeed_ = 0.2f;

	//カメラシェイクの変数
	CameraShakeSettings cameraShakeSettings_{};

	//カメラシェイクのパラメーター
	CameraShakeParameter cameraShakeParameters_{};

	//追従カメラ時のパラメーター
	FollowCameraParameters followCameraParameters_{};

	//ロックオンカメラ時のパラメーター
	LockOnCameraParameters lockOnCameraParameters_{};

	//ダッシュカメラ時のパラメーター
	DashCameraParameters dashCameraParameters_{};
};

