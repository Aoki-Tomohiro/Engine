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
	struct DodgeActionSettings
	{
		float chargeDuration; //溜め時間
		float dodgeDuration;   //回避時間
		float recoveryDuration; //硬直時間
	};

private:
	Input* input_ = nullptr;

	//回避方向
	DodgeDirection dodgeDirection_{};

	//回避用のパラメーター
	std::vector<DodgeActionSettings> dodgeSettings_ =
	{
		{0.26f,0.638f,0.84f},
		{0.28f,0.72f,0.738f},
		{0.26f,0.75f,0.728f},
		{0.26f,0.72f,0.758f},
	};
};

