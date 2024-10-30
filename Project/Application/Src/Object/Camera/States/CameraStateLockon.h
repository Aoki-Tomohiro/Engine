#pragma once
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/Camera/States/ICameraState.h"

/// <summary>
/// ロックオンカメラ状態
/// </summary>
class CameraStateLockon : public ICameraState
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

private:
	/// <summary>
	/// 新しいクォータニオンを計算
	/// </summary>
	/// <returns>新しいクォータニオン</returns>
	const Quaternion CalculateNewRotation() const;

	/// <summary>
	/// カメラとロックオン対象が一定距離以内にいるかどうか
	/// </summary>
	/// <param name="cameraPosition">カメラ座標</param>
	/// <param name="lockOnTargetPosition">ロックオン対象の座標</param>
	/// <returns>カメラとロックオン対象が一定距離以内にいるかどうか</returns>
	const bool IsCameraCloseToTarget(const Vector3& cameraPosition, const Vector3& lockOnTargetPosition) const;

	/// <summary>
	/// カメラの状態遷移
	/// </summary>
	void ManageCameraStateTransition();

private:
	//前のフレームの方向ベクトル
	Vector3 previousDirection_{};

	//現在の方向ベクトル
	Vector3 currentDirection_{};
};

