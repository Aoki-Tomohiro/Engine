#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"

class PlayerStateSpinAttack : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;

private:
	void CheckForAttackStateTransition();

	void UpdateRotation();

	void UpdatePosition();

	void HandleWeaponHit();

	void FinalizeSpinAttack();

private:
	//インプット
	Input* input_ = nullptr;

	//アニメーションの状態
	CombatAnimationState animationState_{};

	//開始座標
	Vector3 startPosition_{};

	//目標座標
	Vector3 targetPosition_{};

	//現在の回転時間
	float currentRotateTime_ = 0.0f;

	//累積回転量
	float preEasedRotation = 0.0f;

	//ヒットタイマー
	float hitTimer_ = 0.0f;

	//ヒットカウント
	int32_t hitCount_ = 0;

	//攻撃状態への遷移するかどうか
	bool isAttackStateTransitionRequested_ = false;
};

