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
	struct ConstGroundAttack
	{
		//振りかぶりの時間
		uint32_t anticipationTime;
		//溜めの時間
		uint32_t chargeTime;
		//攻撃振りの時間
		uint32_t swingTime;
		//硬直時間
		uint32_t recoveryTime;
		//振りかぶりの移動速さ
		Vector3 anticipationSpeed;
		//溜めの移動速さ
		Vector3 chargeSpeed;
		//攻撃振りの移動速さ
		Vector3 swingSpeed;
	};

	//攻撃用ワーク
	struct WorkGroundAttack
	{
		uint32_t attackParameter = 0;
		int32_t comboIndex = 0;
		int32_t inComboPhase = 0;
		bool comboNext = false;
	};

	//コンボ定数表
	static const std::array<ConstGroundAttack, kComboNum> kConstAttacks_;

	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

private:
	void ApplyGlobalVariables();

private:
	Input* input_ = nullptr;

	WorkGroundAttack workAttack_{};
};

