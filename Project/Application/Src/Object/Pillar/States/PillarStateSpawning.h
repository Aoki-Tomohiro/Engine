#pragma once
#include "IPillarState.h"
#include "Engine/Utilities/GameTimer.h"

/// <summary>
/// 柱のアクティブ状態
/// </summary>
class PillarStateSpawning : public IPillarState
{
public:
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

private:
	//完全に出現するまでの時間
	float spawnDuration_ = 0.2f;

	//完全に出現するまでのタイマー
	float spawnTimer_ = 0.0f;
};

