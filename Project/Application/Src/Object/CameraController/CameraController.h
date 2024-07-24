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
		Vector3 justDodgeIntensity = { 0.0f, 10.0f, 0.0f };    // 回避攻撃のカメラシェイクの強度
		Vector3 enhancedMagicIntensity = { 0.0f, 0.8f, 0.0f }; // 強化魔法のカメラシェイクの強度
		float baseDuration = 0.2f;                             // カメラシェイクの持続時間
		float justDodgeDuration = 0.6f;                        // 回避攻撃のカメラシェイクの持続時間
		float enhancedMagicDuration = 0.4f;               // 強化魔法のカメラシェイクの持続時間
	};

	void ChangeState(ICameraState* state);

	void UpdateCameraShake();

	Vector3 Offset();

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

	//追従対象からのオフセット
	Vector3 offset_ { 0.0f, 4.0f, -20.0f };

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

	//フレンドクラスに登録
	friend class CameraStateFollow;
	friend class CameraStateLockOn;
	friend class CameraStateJustDodge;
};

