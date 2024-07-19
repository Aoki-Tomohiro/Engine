#pragma once
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Camera/LockOn.h"
#include "Engine/3D/Transform/WorldTransform.h"
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/CameraController/States/ICameraState.h"

class CameraController
{
public:
	void Initialize();

	void Update();

	void SetTarget(const WorldTransform* target) { target_ = target; };

	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; };

	const Camera& GetCamera() const { return camera_; };

private:
	void ChangeState(ICameraState* state);

private:
	//カメラの状態
	std::unique_ptr<ICameraState> state_ = nullptr;

	//カメラ
	Camera camera_{};

	//ロックオン
	const LockOn* lockOn_ = nullptr;

	//追従対象
	const WorldTransform* target_ = nullptr;

	//追従対象の残像座標
	Vector3 interTarget_{};

	//Quaternion
	Quaternion destinationQuaternion_{ 0.0f,0.0f,0.0f,1.0f };

	//フレンドクラスに登録
	friend class CameraStateFollow;
	friend class CameraStateLockOn;
};

