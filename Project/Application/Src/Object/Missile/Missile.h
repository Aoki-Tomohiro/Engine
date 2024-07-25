#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Components/Particle/ParticleManager.h"

class Missile : public GameObject
{
public:
	//カウンター
	static int32_t counter;

	struct MissileParameters
	{
		float interpolationRate = 1.0f;         // 球面線形補間の速度
		float maxSlerpFactor = 0.06f;           // 球面線形補間の最大係数
		float trackingTimeLimit = 4.0f;         // 追尾完了までの最大時間　
		float speedMultiplier = 26.0f;          // 速度の倍率を示す定数
	};

	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void DrawUI() override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	void SetMissileParameters(const MissileParameters& missileParameters) { missileParameters_ = missileParameters; };

	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; };

private:
	//自分のID
	int32_t id_ = 0;

	//速度
	Vector3 velocity_{};

	//ミサイルのパラメーター
	MissileParameters missileParameters_{};

	//追尾用の媒介変数
	float slerpProgress_ = 0.0f;

	//追尾が終了しているか
	bool isTrackingComplete_ = false;

	//追尾タイマー
	float trackingTimer_ = 0.0f;

	//パーティクル
	ParticleSystem* particleSystem_ = nullptr;
};

