#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/AnimationStateManager/AnimationStateManager.h"

class PlayerStateAttack : public IPlayerState
{
public:
	//攻撃用ワーク
	struct WorkAttack
	{
		float attackParameter = 0.0f;        //攻撃用のパラメーター
		int32_t comboIndex = 0;              //現在のコンボのインデックス
		int32_t inComboPhase = 0;            //現在のコンボの中のフェーズ
		bool comboNext = false;              //次のコンボが有効かどうか
		bool isMovementFinished = false;     //移動が終了しているかどうか
		bool hasHitTargetThisAttack = false; //攻撃が当たったかどうか
		bool isAerialAttack = false;         //空中攻撃状態かどうか
	};

	void Initialize() override;

	void Update() override;

private:
	void EvaluateComboProgress();

	float GetSwingTime(uint32_t comboIndex);

	void HandleComboTransition();

	void UpdateComboState();

	void UpdateAnimationAndHandlePhase();

	void RotatePlayerTowardsEnemy();

	void HandleComboPhase(int phase);

	void ChargeUpdate();

	void SwingUpdate();

	void RecoveryUpdate();

private:
	//Input
	Input* input_ = nullptr;

	//地上攻撃用ワーク
	WorkAttack workAttack_{};

	//速度
	Vector3 velocity_{};

	//アニメーションの状態
	std::vector<AnimationState> states_{};

	//フェーズの名前
	std::string phaseName_{};
};

