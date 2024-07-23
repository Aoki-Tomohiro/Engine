#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Particle/ParticleManager.h"

class PlayerStateDash : public IPlayerState
{
public:
	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	void OnCollision(GameObject* other);

	void OnCollisionEnter(GameObject* other);

	void OnCollisionExit(GameObject* other);

private:
	struct WorkDash
	{
		float dashTimer = 0.0f;          // ダッシュ用タイマー
		bool isDashFinished = false;     // ダッシュが終了したか
		float chargeTimer = 0.0f;        // チャージ用タイマー
		bool isChargingFinished = false; // チャージが終了したか
	};

private:
	Input* input_ = nullptr;

	WorkDash dashWork_{};

	ParticleSystem* particleSystem_ = nullptr;
};

