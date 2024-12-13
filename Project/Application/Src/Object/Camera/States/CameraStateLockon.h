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
	/// 新しいクォータニオンを計算
	/// </summary>
	/// <returns>新しいクォータニオン</returns>
	Quaternion CalculateNewRotation() const;

	/// <summary>
	/// 追従対象とロックオン対象が一定距離以内にいるかどうか
	/// </summary>
	/// <param name="followPosition">追従対象座標</param>
	/// <param name="lockonPosition">ロックオン対象の座標</param>
	/// <returns>追従対象とロックオン対象が一定距離以内にいるかどうか</returns>
	bool IsWithinLockonDistance(const Vector3& followPosition, const Vector3& lockonPosition);

private:
	//前のフレームの方向ベクトル
	Vector3 previousDirection_{};

	//現在の方向ベクトル
	Vector3 currentDirection_{};
};

