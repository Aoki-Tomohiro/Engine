#pragma once
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Camera/LockOn.h"
#include "Engine/3D/Transform/WorldTransform.h"
#include "Engine/Components/Input/Input.h"

class CameraController
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 追従対象を設定
	/// </summary>
	/// <param name="target"></param>
	void SetTarget(const WorldTransform* target);

	/// <summary>
	/// ロックオンを設定
	/// </summary>
	/// <param name="lockOn"></param>
	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; };

	/// <summary>
	/// 目標オフセットを設定
	/// </summary>
	/// <param name="destinationOffset"></param>
	void SetDestinationOffset(const Vector3& destinationOffset) { destinationOffset_ = destinationOffset; };

	/// <summary>
	/// カメラを取得
	/// </summary>
	/// <returns></returns>
	const Camera& GetCamera() const { return camera_; };

private:
	//カメラシェイク用の構造体
	struct CameraShakeSettings
	{
		bool isShaking = false;// カメラシェイクが有効かどうかのフラグ
		Vector3 intensity = { 0.0f, 0.6f, 0.0f };// カメラシェイクの強度
		Vector3 offset = { 0.0f, 0.0f, 0.0f };// カメラシェイクの基準座標オフセット
		float duration = 0.1f;// カメラシェイクの持続時間
		float timer = 0.0f;// カメラシェイクの経過時間を計測するタイマー
	};

	/// <summary>
	/// Offsetを計算
	/// </summary>
	/// <returns></returns>
	Vector3 Offset();

	/// <summary>
	/// カメラリセットの処理
	/// </summary>
	void Reset();

	/// <summary>
	/// カメラシェイクの処理
	/// </summary>
	void UpdateCameraShake();

	/// <summary>
	/// 環境変数の適用
	/// </summary>
	void ApplyGlobalVariables();

private:
	//Input
	Input* input_ = nullptr;

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

	//追従対象からのオフセット
	Vector3 offset_{ 0.0f, 2.0f, -20.0f };

	//目標オフセット
	Vector3 destinationOffset_{ 0.0f, 2.0f, -20.0f };

	//カメラシェイクの変数
	CameraShakeSettings cameraShakeSettings_{};
};

