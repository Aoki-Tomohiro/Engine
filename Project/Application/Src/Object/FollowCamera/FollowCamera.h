#pragma once
#include "Engine/Components/Input/Input.h"
#include "Engine/3D/Model/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Math/MathFunction.h"

class LockOn;

class FollowCamera
{
public:
	void Initialize();

	void Update();

	void SetTarget(const WorldTransform* target);

	const Camera& GetCamera() { return camera_; };

	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; };

	const int GetCameraSensitivity() const { return cameraSensitivity_; };

	void SetCameraSensitivity(const int cameraSensitivity) { cameraSensitivity_ = cameraSensitivity; };

	void SetClearAnimation(const bool clearAnimation) { clearAnimation_ = clearAnimation; };

	const bool GetClearAnimationEnd() const { return clearAnimationEnd_; };

private:
	Vector3 Offset();

	void Reset();

private:
	//入力クラス
	Input* input_ = nullptr;

	//カメラ
	Camera camera_{};

	//目標角度
	float destinationAngleX_ = 0.0f;
	float destinationAngleY_ = 0.0f;

	//追従対象の残像座標
	Vector3 interTarget_{};

	//追従対象
	const WorldTransform* target_{};

	//ロックオン
	const LockOn* lockOn_ = nullptr;

	//カメラの回転速度
	float rotSpeedY_ = 0.06f;

	//カメラの速度設定
	int cameraSensitivity_ = 3;

	//オフセット
	Vector3 offset_ = { 0.0f, 2.0f, -30.0f };

	//クリアアニメーションのフラグ
	bool clearAnimation_ = false;
	bool clearAnimationEnd_ = false;
};

