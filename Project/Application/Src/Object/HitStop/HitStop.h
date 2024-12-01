#pragma once
#include "Engine/Utilities/GameTimer.h"

/// <summary>
/// ヒットストップ
/// </summary>
class HitStop
{
public:
	/// <summary>
	/// ヒットストップを開始
	/// </summary>
	/// <param name="duration">持続時間</param>
	void Start(const float duration);

	/// <summary>
	/// ヒットストップを停止
	/// </summary>
	void Stop();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

private:
	//残り時間
	float remainingTime_ = 0.0f;

	//ヒットストップがアクティブかどうか
	bool isActive_ = false;
};

