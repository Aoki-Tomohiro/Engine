#pragma once
#include "IPillarState.h"
#include "Engine/Utilities/GameTimer.h"

/// <summary>
/// 柱の消失状態
/// </summary>
class PillarStateDespawning : public IPillarState
{
public:
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

private:
	//消失するまでの時間
	float despawnDuration_ = 1.0f;

	//消失するまでのとタイマー
	float despawnTimer_ = 0.0f;

	//透明度
	float alpha_ = 1.0f;
};

