#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"

class PlayerStateDodge : public IPlayerState
{
public:
	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	void OnCollision(GameObject* other);

	void OnCollisionEnter(GameObject* other);

	void OnCollisionExit(GameObject* other);

private:
	//回避方向の列挙型
	enum DodgeDirection
	{
		Forward,
		Backward,
		Left,
		Right,
		NumDirections // 方向の数を表す定数
	};

	//回避用のパラメーターの構造体
	struct DodgeDurations
	{
		float chargeDuration; // 溜め時間
		float dodgeDuration;   // 回避時間
		float recoveryDuration; // 硬直時間
	};

	//回避の時間設定
	static const std::array<DodgeDurations, NumDirections> kDodgeDurations_;

private:
	Input* input_ = nullptr;

	//回避方向
	DodgeDirection dodgeDirection_{};
};

