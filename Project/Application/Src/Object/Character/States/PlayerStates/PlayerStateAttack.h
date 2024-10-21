#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"

class PlayerStateAttack : public IPlayerState
{
public:
	//攻撃の状態
	enum AttackState
	{
		kCharge,
		kAttack,
		kRecovery,
	};

	//次の攻撃
	enum NextState
	{
		kNormalCombo,
		kDash,
		kLaunchAttack,
		kSpinAttack,
		kFallingAttack,
		kChargeMagic,
		kStomp,
	};

	//攻撃用ワーク
	struct WorkAttack
	{
		float attackParameter = 0.0f;                 //攻撃用のパラメーター
		int32_t hitCount = 0;                         //ヒットカウント
		int32_t comboIndex = 0;                       //現在のコンボのインデックス
		int32_t inComboPhase = 0;                     //現在のコンボの中のフェーズ
		bool comboNext = false;                       //次のコンボが有効かどうか
		NextState comboNextState = kNormalCombo;      //次のコンボの状態
		bool isMovementFinished = false;              //移動が終了しているかどうか
	};

	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;

private:
	void PlayAttackAnimation(const bool isAerial);

	void LoadAnimationStates(const bool isAerial);

	void InitializeDashAttack();

	void CalculateVelocity();

	void EvaluateComboProgress();

	void UpdateComboStateBasedOnButtonPress();

	void UpdateComboStateWithoutTrigger();

	void EnableCombo(const NextState& nextState);

	void UpdateAnimationAndHandlePhase();

	void HandleComboPhase();

	void ChargeUpdate();

	void AttackUpdate();

	void RecoveryUpdate();

	void RotatePlayerTowardsEnemy();

	void UpdateComboState();

	float GetAttackDuration();

	void HandleComboTransition();

	void CorrectPlayerPosition();

	void HandleNormalComboTransition();

	void ResetAttackWork();

	void HandleDirectionalChange();

	void FinalizeAttackState();

private:
	//インプット
	Input* input_ = nullptr;

	//攻撃用ワーク
	WorkAttack workAttack_{};

	//速度
	Vector3 velocity_{};

	//アニメーションの状態
	std::vector<CombatAnimationState> animationStates_{};

	//ボタンが押されている時間
	float buttonPressedTime_ = 0.0f;

	//同時押しと判定する時間
	const float kSimultaneousPressThreshold_ = 0.1f;
};

