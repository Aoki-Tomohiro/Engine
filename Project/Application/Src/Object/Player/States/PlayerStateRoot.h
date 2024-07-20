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
	//ジャスト回避用の構造体
	struct JustDodgeSettings
	{
		bool isInWarningRange = false; // 警告範囲に入っているか
		bool isJustDodgeAvailable = false;// ジャスト回避が可能かどうか
		float justDodgeTimer = 0.0f;// ジャスト回避用のタイマー
	};

	void UpdateStickInput();

	void UpdateMovement();

	void UpdateRotation();

	void UpdateLockOnAnimation();

	void UpdateIdleAnimation();

	void UpdateJustDodge();

private:
	Input* input_ = nullptr;

	JustDodgeSettings justDodgeSettings_{};

	Vector3 inputValue_{};

	float inputLength_ = 0.0f;

	bool isRunning_ = false;
};

