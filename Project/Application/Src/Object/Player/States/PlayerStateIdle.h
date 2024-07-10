#pragma once
#include "IPlayerState.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GlobalVariables.h"

class PlayerStateIdle : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void OnCollision(GameObject* other) override;

	void OnCollisionEnter(GameObject* other) override;

	void OnCollisionExit(GameObject* other) override;

private:
	//ジャスト回避用の構造体
	struct JustDodgeSettings
	{
		float maxJustDodgeDistance = 10.0f;// ジャスト回避が成功する距離
		bool isJustDodgeAvailable = false;// ジャスト回避が可能かどうか
		bool isJustDodgeSuccess = false;// ジャスト回避に成功したかどうか
		float justDodgeDuration = 1.0f;// ジャスト回避の成功時間
		float justDodgeTimer = 0.0f;// ジャスト回避用のタイマー
		bool isCounterAttackSuccess = false;
		bool isCounterAttackEnded = false; // カウンター攻撃の受付が終了したかどうか
		float counterAttackDuration = 3.0f;// カウンター攻撃の受付時間
		float counterAttackTimer = 0.0f;// カウンター攻撃用のタイマー
	};

	void UpdateJustDodge();

	void ApplyGlobalVariables();

private:
	Input* input_ = nullptr;

	float moveSpeed_ = 24.0f;

	JustDodgeSettings justDodgeSettings_{};
};

