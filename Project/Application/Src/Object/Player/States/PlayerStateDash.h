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
	//ダッシュ用の構造体
	struct DashWork
	{
		float proximityThreshold = 8.0f; //移動を止める距離
		float dashDuration = 0.5f; //ダッシュ時間
		float dashTimer = 0.0f; //ダッシュ用タイマー
		bool isDashFinished = false; //ダッシュが終了したか
		float chargeDuration = 0.16f; //チャージ時間
		float chargeTimer = 0.0f; //チャージ用タイマー
		bool isChargingFinished = false; //チャージが終了したか
	};

private:
	Input* input_ = nullptr;

	DashWork workDash_{};

	ParticleSystem* particleSystem_ = nullptr;
};

