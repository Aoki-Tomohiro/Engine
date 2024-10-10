#pragma once
#include "IPillarState.h"
#include "Engine/Utilities/GameTimer.h"

/// <summary>
/// 柱の非アクティブ状態
/// </summary>
class PillarStateInactive : public IPillarState
{
public:
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

private:
	//非アクティブのタイマー
	float inactiveTimer_ = 0.0f;
};

