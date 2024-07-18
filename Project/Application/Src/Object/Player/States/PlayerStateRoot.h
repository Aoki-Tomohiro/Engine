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
		float maxJustDodgeDistance = 10.0f;// ジャスト回避が成功する距離
		bool isJustDodgeAvailable = false;// ジャスト回避が可能かどうか
		bool isJustDodgeSuccess = false;// ジャスト回避に成功したかどうか
		float justDodgeDuration = 1.0f;// ジャスト回避の成功時間
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

