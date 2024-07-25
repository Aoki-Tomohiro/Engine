#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Application/Src/Object/Enemy/States/IEnemyState.h"
#include "Application/Src/Object/Warning/Warning.h"
#include "Application/Src/Object/Missile/Missile.h"

class Enemy : public GameObject
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void DrawUI() override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	const bool GetIsWarning() const { return state_->GetIsWarning(); };

	const bool GetIsAttack() const { return state_->GetIsAttack(); };

	void SetTimeScale(const float timeScale) { timeScale_ = timeScale; };

	void SetIsInTitleScene(const bool isInTitleScene) { isInTitleScene_ = isInTitleScene; };

	void SetCamera(const Camera* camera) { camera_ = camera; };

	void SetDestinationQuaternion(const Quaternion& quaternion) { destinationQuaternion_ = quaternion; };

private:
	//通常状態のパラメーター
	struct RootParameters
	{
		float moveSpeed = 7.0f;           // 移動速度
		float minActionInterval_ = 2.0f;  // 攻撃までの最小時間
		float maxActionInterval_ = 4.0f;  // 攻撃までの最大時間
		float stopDistance = 10.0f;       // 動きを止める距離
		float closeRangeDistance = 20.0f; // 遠距離の行動をする距離
	};

	//タックル状態のパラメーター
	struct TackleParameters
	{
		float chargeDuration_ = 0.8f;  // 溜め時間
		float warningDuration_ = 1.0f; // 攻撃予告時間
		float attackDuration_ = 1.7f;  // 攻撃時間
		float targetDistance = 20.0f;
	};

	//ミサイル攻撃状態のパラメーター
	struct MissleAttackParameters
	{
		float chargeDuration_ = 2.0f;                             // 溜め時間
		float fireInterval = 0.2f;                                // 発射間隔
		float recoveryDuration_ = 1.0f;                           // 硬直時間
		std::vector<Missile::MissileParameters> missileParameters //ミサイルのパラメーター
		{
			{ 1.0f,0.06f,4.0f,26.0f},
			{ 1.0f,0.06f,4.0f,26.0f},
			{ 1.0f,0.06f,4.0f,26.0f},
		};
	};

	void ChangeState(IEnemyState* state);

	void CreateWarningParticle();

	Warning* CreateBoxWarningObject(const Warning::BoxWarningObjectSettings& warningObjectSettings);

private:
	//敵の状態
	std::unique_ptr<IEnemyState> state_ = nullptr;

	//速度
	Vector3 velocity{};

	//Quaternion
	Quaternion destinationQuaternion_{ 0.0f,0.0f,0.0f,1.0f };

	//TimeScale
	float timeScale_ = 1.0f;

	//通常状態のパラメーター
	RootParameters rootParameters_{};

	//タックル状態のパラメーター
	TackleParameters tackleParameters_{};

	//ミサイル攻撃のパラメーター
	MissleAttackParameters missileAttackParameters_{};

	//パーティクル
	ParticleSystem* particleSystem_ = nullptr;

	//アクティブ状態かどうか
	bool isMove_ = false;

	//タイトルシーンなのか
	bool isInTitleScene_ = false;

	//Debugモードなのか
	bool isDebug_ = false;

	//アニメーションの時間
	float animationTime_ = 0.0f;

	//Camera
	const Camera* camera_ = nullptr;

	//フレンドクラスに登録
	friend class EnemyStateRoot;
	friend class EnemyStateTackle;
	friend class EnemyStateMissile;
};

