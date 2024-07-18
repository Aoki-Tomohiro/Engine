#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"

class PlayerStateGroundAttack : public IPlayerState
{
public:
	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	void OnCollision(GameObject* other);

	void OnCollisionEnter(GameObject* other);

	void OnCollisionExit(GameObject* other);

private:
	//コンボの数
	static const int kComboNum = 4;

	//攻撃用定数
	struct ConstGroundAttack
	{
		//溜めの時間
		float chargeTime;
		//攻撃振りの時間
		float swingTime;
		//硬直時間
		float recoveryTime;
		//移動速度
		float moveSpeed;
	};

	//攻撃用ワーク
	struct WorkGroundAttack
	{
		float attackParameter = 0.0f;
		int32_t comboIndex = 0;
		int32_t inComboPhase = 0;
		bool comboNext = false;
	};

	//コンボ定数表
	static const std::array<ConstGroundAttack, kComboNum> kConstAttacks_;

private:
	//Input
	Input* input_ = nullptr;

	//攻撃用ワーク
	WorkGroundAttack workAttack_{};

	//攻撃の補正を掛ける距離の閾値
	float attackThreshold_ = 8.0f;

	//パリィ用のタイマー
	float parryWindow_ = 0.0f;
};

