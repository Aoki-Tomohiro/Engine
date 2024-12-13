/**
 * @file CameraShake.h
 * @brief カメラシェイクを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GameTimer.h"
#include "Engine/Utilities/RandomGenerator.h"

class CameraShake
{
public:
	/// <summary>
	/// カメラシェイク開始
	/// </summary>
	/// <param name="intensity">強さ</param>
	/// <param name="duration">長さ</param>
	void Start(const Vector3& intensity, const float duration);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// カメラシェイクのオフセット値を取得
	/// </summary>
	/// <returns>カメラシェイクのオフセット値</returns>
	const Vector3& GetShakeOffset() const { return shakeOffset_; };

private:
	//カメラシェイクがアクティブかどうか
	bool isActive_ = false;

	//カメラシェイクの持続時間
	float duration_ = 0.2f;

	//経過時間
	float elapsedTime_ = 0.0f;

	//カメラシェイクの強度
	Vector3 intensity_{};

	//カメラシェイクのオフセット
	Vector3 shakeOffset_{};
};

