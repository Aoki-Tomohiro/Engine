#pragma once
#include "IEnemyState.h"

class EnemyStateRoot : public IEnemyState
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

private:
	//近距離の行動
	enum CloseRangeAction
	{
		kTackle,
		kMaxCloseRangeActions
	};

	//遠距離の行動
	enum LongRangeAction
	{
		kDash,
		kMagicAttack,
		kMaxLongRangeActions
	};

private:
	//現在の次の行動の間隔
	float currentActionInterval_ = 1.0f;

	//アクションタイマー
	float actionTimer_ = 0.0f;
};

