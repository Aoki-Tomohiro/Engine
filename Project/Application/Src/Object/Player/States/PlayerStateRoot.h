#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"

class PlayerStateRoot : public IPlayerState
{
public:
	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	void OnCollision(GameObject* other);

	void OnCollisionEnter(GameObject* other);

	void OnCollisionExit(GameObject* other);

private:
	void UpdateStickInput();

	void UpdateMovement();

	void UpdateRotation();

	void UpdateLockOnAnimation();

	void UpdateIdleAnimation();

	void UpdateJustDodge();

	void UpdateMagicProjectileAttack();

	//ジャスト回避用の構造体
	struct WorkJustDodge
	{
		bool isInWarningRange = false;         // 警告範囲に入っているか
		bool isJustDodgeAvailable = false;     // ジャスト回避が可能かどうか
		float justDodgeSuccessTimer = 0.0f;    // ジャスト回避が成功したかを判別するタイマー
	};

	//魔法攻撃用の構造体
	struct WorkMagicAttack
	{
		bool isMagicAttack_ = false;         // 魔法攻撃のフラグ
		float fireTimer_ = 0.0f;             // 魔法攻撃用のタイマー
		float finishedTimer_ = 0.0f;         // 魔法攻撃が終了してるかを判別するタイマー
	};

private:
	//Input
	Input* input_ = nullptr;

	//スティックの入力値
	Vector3 inputValue_{};

	//入力の強さ
	float inputLength_ = 0.0f;

	//走り状態かどうか
	bool isRunning_ = false;

	//ジャスト回避用ワーク
	WorkJustDodge justDodgeWork_{};

	//魔法攻撃用ワーク
	WorkMagicAttack workMagicAttack_{};
};

