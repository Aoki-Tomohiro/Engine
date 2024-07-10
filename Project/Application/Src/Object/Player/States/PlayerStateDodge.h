#pragma once
#include "IPlayerState.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GlobalVariables.h"

class PlayerStateDodge : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void OnCollision(GameObject* other) override;

	void OnCollisionEnter(GameObject* other) override;

	void OnCollisionExit(GameObject* other) override;

private:
	void ApplyGlobalVariables();

private:
	//Input
	Input* input_ = nullptr;

	//回避スピード
	float dodgeSpeed_ = 24.0f;

	//バックフリップ状態なのか
	bool isBackFlip_ = false;

	//バックフリップのため状態の時間
	float backFlipAnticipationTime = 0.2f;

	//バックフリップ状態の時間
	float backFlipTime = 0.6f;

	//バックフリップ状態のタイマー
	float backFlipParameter_ = 0.0f;

	//バックフリップの状態
	enum BackFlipState
	{
		kAnticipation,
		kBackFlip,
		kRecovery,
	};
	BackFlipState backFlipState_ = BackFlipState::kAnticipation;
};

