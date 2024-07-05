#pragma once
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Camera/LockOn.h"
#include "Engine/3D/Transform/WorldTransform.h"
#include "Engine/Components/Input/Input.h"

class CameraController
{
public:
	void Initialize();

	void Update();

	void UpdateCameraShake();

	void SetTarget(const WorldTransform* target);

	const Camera& GetCamera() const { return camera_; };

	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; };

	void SetDestinationOffset(const Vector3& offset) { destinationOffset_ = offset; };

	void SetIsClearAnimation(const bool isClearAnimation) { isClearAnimation_ = isClearAnimation; };

	const bool GetClearAnimationEnd() { return clearAnimationEnd_; };

private:
	void UpdateFollowCamera();

	Vector3 Offset();

	void Reset();

	void ApplyGlobalVariables();

private:
	//カメラ
	Camera camera_{};

	//ロックオン
	const LockOn* lockOn_ = nullptr;

	//追従対象
	const WorldTransform* target_ = nullptr;

	//追従対象の残像座標
	Vector3 interTarget_{};

	//目標角度
	float destinationAngleX_ = 0.0f;
	float destinationAngleY_ = 0.0f;

	Vector3 destinationOffset_{ 0.0f, 2.0f, -20.0f };
	Vector3 offset_{ 0.0f, 2.0f, -20.0f };

	//カメラシェイクの変数
	bool isShaking_ = false;
	Vector3 shakeIntensity_{ 0.0f,0.6f,0.0f };
	Vector3 shakeoffset_{ 0.0f,0.0f,0.0f };
	float shakeDuration_ = 0.1f;
	float shakeTimer_ = 0.0f;

	//クリアアニメーション
	bool isClearAnimation_ = false;
	bool clearAnimationEnd_ = false;
};

