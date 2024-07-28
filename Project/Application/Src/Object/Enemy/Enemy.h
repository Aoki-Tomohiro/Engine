#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/2D/Sprite.h"
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

	const bool GetIsDead() const { return isDead_; };

	const float GetDamage() const { return damage_; };

	const float GetKnockbackSpeed() const { return knockbackSpeed_; };

	void SetIsParrying(const bool isParrying) { isParrying_ = isParrying; };

private:
	//攻撃のパラメーター
	struct AttackParameters
	{
		float chargeDuration = 0.0f;   // 溜め時間
		float warningDuration = 0.0f;  // 攻撃予告時間
		float attackDuration = 0.0f;   // 攻撃時間
		float knockbackSpeed = -1.0f;  // ノックバックさせる速度
		float damage = 1.0f;           // ダメージ
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
		AttackParameters attackParameters = { 1.1f,1.2f,1.7f,-96.0f,10.0f }; // 攻撃用のパラメーター
		float targetDistance = 20.0f;                                        // 目標までの距離
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
		AttackParameters attackParameters = { 0.78f,1.2f,3.3f,-6.0f,6.0f }; // 攻撃用のパラメーター
		float laserRange = 30.0f;                                            // レーザーの長さ
		float easingSpeed = 10.0f;                                           // イージングの速度
		float damage = 1.0f;                                                 // ダメージ
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
		AttackParameters attackParameters = { 0.96f,1.1f,1.8f,-96.0f,20.0f }; // 攻撃用のパラメーター
		float trackingDistance = 14.0f;                                       // 追尾する距離
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
			{0.68f,0.9f, 1.1f,-28.0f,10.0f},
			{1.1f, 1.48f,1.7f,-28.0f,10.0f},
			{1.7f, 2.4f, 2.7f,-68.0f,20.0f},
		};
	};

	//死亡状態のパラメーター
	struct DeadParameters
	{
		float deathTime = 1.0f; // 死亡までの時間
		float edgeWidth = 0.1f; // DissolveのEdgeWidth
	};

	void ChangeState(IEnemyState* state);

	void CreateWarningParticle();

	void UpdateHP();

	Warning* CreateBoxWarningObject(const Warning::BoxWarningObjectSettings& warningObjectSettings);

private:
	//Audio
	Audio* audio_ = nullptr;

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

	//死亡状態のパラメーター
	DeadParameters deadParameters_{};

	//ダメージ
	float damage_ = 1.0f;

	//ノックバックさせる速度
	float knockbackSpeed_ = 1.0f;

	//パリィ状態かどうか
	bool isParrying_ = false;

	//死亡フラグ
	bool isDead_ = false;

	//体力
	const float kMaxHP = 800.0f;
	float hp_ = kMaxHP;

	//パーティクル
	ParticleSystem* particleSystem_ = nullptr;

	//体力のスプライト
	std::unique_ptr<Sprite> hpSprite_ = nullptr;
	std::unique_ptr<Sprite> hpFrameSprite_ = nullptr;
	Vector2 hpSpritePosition_ = { 720.0f,32.0f };
	Vector2 hpSpriteSize_{ 480.0f,16.0f };
	Vector2 hpFrameSpritePosition_ = { 719.0f,31.0f };

	//オーディオハンドル
	uint32_t tackleAudioHandle_ = 0;
	uint32_t comboAudioHandle1_ = 0;
	uint32_t comboAudioHandle2_ = 0;
	uint32_t comboAudioHandle3_ = 0;
	uint32_t jumpAttackAudioHandle_ = 0;
	uint32_t chargeAudioHandle_ = 0;
	uint32_t laserAudioHandle_ = 0;

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
	friend class EnemyStateDead;
};

