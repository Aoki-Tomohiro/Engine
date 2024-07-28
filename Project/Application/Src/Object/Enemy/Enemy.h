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

	const Vector3 GetHipLocalPosition();

	const Vector3 GetHipWorldPosition();

private:
	//攻撃のパラメーター
	struct AttackParameters
	{
		float chargeDuration = 0.0f;  // 溜め時間
		float warningDuration = 0.0f; // 攻撃予告時間
		float attackDuration = 0.0f;  // 攻撃時間
	};

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
		AttackParameters attackParameters = { 0.8f,1.0f,1.7f }; // 攻撃用のパラメーター
		float targetDistance = 20.0f;                           // 目標までの距離
	};

	//ミサイル攻撃状態のパラメーター
	struct MissleAttackParameters
	{
		AttackParameters attackParameters = { 0.8f,0.0f,3.3f };   // 攻撃用のパラメーター
		int32_t maxFireCount = 6;                                 // 弾の最大発射数
		std::vector<Missile::MissileParameters> missileParameters //ミサイルのパラメーター
		{
			{ 1.0f,0.06f,4.0f,26.0f},
			{ 1.0f,0.06f,4.0f,26.0f},
			{ 1.0f,0.06f,4.0f,26.0f},
			{ 1.0f,0.06f,4.0f,26.0f},
			{ 1.0f,0.06f,4.0f,26.0f},
			{ 1.0f,0.06f,4.0f,26.0f},
		};
	};

	//レーザー攻撃状態のパラメーター
	struct LaserAttackParameters
	{
		AttackParameters attackParameters = { 0.78f,1.2f,3.3f };   // 攻撃用のパラメーター
		float laserRange = 30.0f;                                 // レーザーの長さ
		float easingSpeed = 10.0f;                                // イージングの速度
	};

	//ダッシュ状態のパラメーター
	struct DashParameters
	{
		float dashSpeed = 26.0f;      // ダッシュスピード
		float attackDistance = 14.0f; // 攻撃状態に遷移する距離
	};

	//ジャンプ攻撃用のパラメーター
	struct JumpAttackParameters
	{
		AttackParameters attackParameters = { 0.96f,1.1f,1.8f }; // 攻撃用のパラメーター
		float trackingDistance = 14.0f;                          // 追尾する距離
	};

	//コンボ攻撃のパラメーター
	struct ComboAttackParameters
	{
		int32_t maxComboIndex = 3;                       // コンボ数
		float moveDistance = 16.0f;                      // 移動させる距離
		std::vector<Vector3> attackVelocity =            // 攻撃時の移動速度
		{
			{0.0f,0.0f,28.0f},
			{0.0f,0.0f,28.0f},
			{0.0f,0.0f,28.0f},
		};
		std::vector<AttackParameters> attackParameters = // 攻撃用のパラメーター
		{
			{0.68f,0.9f,1.1f},
			{1.1f,1.2f,1.7f},
			{1.7f,2.4f,2.7f},
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

	//回転の保管速度
	float quaternionInterpolationSpeed_ = 0.4f;

	//TimeScale
	float timeScale_ = 1.0f;

	//通常状態のパラメーター
	RootParameters rootParameters_{};

	//タックル状態のパラメーター
	TackleParameters tackleParameters_{};

	//ミサイル攻撃のパラメーター
	MissleAttackParameters missileAttackParameters_{};

	//レーザー攻撃のパラメーター
	LaserAttackParameters laserAttackParameters_{};

	//ダッシュのパラメーター
	DashParameters dashParameters_{};

	//ジャンプ攻撃用のパラメーター
	JumpAttackParameters jumpAttackParameters_{};

	//コンボ攻撃用のパラメーター
	ComboAttackParameters comboAttackParameters_{};

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
	friend class EnemyStateDash;
	friend class EnemyStateTackle;
	friend class EnemyStateMissile;
	friend class EnemyStateJumpAttack;
	friend class EnemyStateComboAttack;
	friend class EnemyStateLaserAttack;
};

