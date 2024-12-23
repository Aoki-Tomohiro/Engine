/**
 * @file CameraStateLockon.h
 * @brief ロックオンカメラの状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/Camera/States/ICameraState.h"

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
	/// 新しいY軸のクォータニオンを計算
	/// </summary>
	/// <param name="directionVector">方向ベクトル</param>
	/// <returns>新しいY軸のクォータニオン</returns>
	Quaternion CalculateNewRotationY(const Vector3& directionVector) const;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="directionVector">方向ベクトル</param>
	/// <param name="quaternionY">Y軸のクォータニオン</param>
	/// <returns></returns>
	Quaternion CalculateNewRotationX(const Vector3& directionVector, const Quaternion& quaternionY) const;
};

