#pragma once
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/RandomGenerator.h"
#include "Application/Src/Object/Enemy/States/IEnemyState.h"

class EnemyStateRoot : public IEnemyState
{
public:
	//近距離の行動
	enum CloseRangeAction
	{
		kTackle,
		kJumpAttack,
		kComboAttack,
		kMaxCloseRangeActions
	};

	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;

private:
	void UpdatePositionAndAnimation();

	void MoveTowardsPlayer(const Vector3& direction);

	void PlayIdleAnimationIfNotPlaying();

	void PlayAnimationIfNotPlaying(const std::string& animationName);

	void RotateTowardsPlayer();

	void PerformNextAction();

	int ChooseNextAction() const;

	void ExecuteAction(int action);

private:
	//前のアクション状態
	static int preAction_;

	//現在のアニメーション
	std::string currentAnimation_ = "Idle";

	//速度
	Vector3 velocity_{};

	//現在の次の行動の間隔
	float currentActionInterval_ = 1.0f;

	//アクションタイマー
	float actionTimer_ = 0.0f;

	//現在の選択中の状態
	int currentItem_ = kMaxCloseRangeActions;

	//次のアクションが有効かどうか
	bool isActionCooldownComplete = false;
};

