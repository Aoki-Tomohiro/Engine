#pragma once
#include "Engine/Utilities/GameTimer.h"

class HitStop
{
public:
	void Start(const float duration);

	void Update();

private:
	//残り時間
	float remainingTime_ = 0.0f;

	//ヒットストップがアクティブかどうか
	bool isActive_ = false;
};

