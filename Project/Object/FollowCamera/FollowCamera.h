#pragma once
#include "Engine/3D/WorldTransform.h"
#include "Engine/3D/Camera.h"
#include "Engine/Components/Input.h"

class LockOn;

class FollowCamera
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
	/// 追従対象をセット
	/// </summary>
	/// <param name="target"></param>
	void SetTarget(const WorldTransform* target);

	/// <summary>
	/// カメラを取得
	/// </summary>
	/// <returns></returns>
	const Camera& GetCamera() { return camera_; };

	/// <summary>
	/// ロックオンをセット
	/// </summary>
	/// <param name="lockOn"></param>
	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; };

private:
	/// <summary>
	/// オフセット計算
	/// </summary>
	/// <returns></returns>
	Vector3 Offset();

	/// <summary>
	/// リセット
	/// </summary>
	void Reset();

private:
	//入力クラス
	Input* input_ = nullptr;

	//カメラ
	Camera camera_{};

	//追従対象
	const WorldTransform* target_{};

	//追従対象の残像座標
	Vector3 interTarget_{};

	//目標角度
	float destinationAngleX_ = 0.0f;
	float destinationAngleY_ = 0.0f;

	//ロックオン
	const LockOn* lockOn_ = nullptr;
};

