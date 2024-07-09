#pragma once
#include "IPlayerState.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Engine/Math/MathFunction.h"
#include <array>

class PlayerStateGroundAttack : public IPlayerState
{
public:
	//コンボの数
	static const int kComboNum = 4;

	//攻撃用定数

	//攻撃用定数
	struct ConstGroundAttack
	{
		//振りかぶりの時間
		float anticipationTime;
		//溜めの時間
		float chargeTime;
		//攻撃振りの時間
		float swingTime;
		//硬直時間
		float recoveryTime;
		//当たり判定の間隔
		float hitInterval;
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

	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	const uint32_t GetComboIndex() const { return workAttack_.comboIndex; };

private:
	void ApplyGlobalVariables();

private:
	Input* input_ = nullptr;

	WorkGroundAttack workAttack_{};

	float parrySuccessTime_ = 0.1f;

	float parryWindow_ = 0.0f;
};

