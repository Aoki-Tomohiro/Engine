#pragma once
#include "Engine/3D/Model/AnimationManager.h"
#include "Application/Src/Object/Character/ICharacter.h"
#include "Application/Src/Object/Character/States/EnemyStates/IEnemyState.h"

//TODO:各状態にコピペコードが多いのでまとめる

class Enemy : public ICharacter
{
public:
	//攻撃タイプ
	enum class AttackType
	{
		kCloseRange,
		kRanged,
		kMaxAttackTypes,
	};

	//距離を詰めるアクション
	enum class ApproachAction
	{
		kDashForward,
		kRunTowardsPlayer,
		kMaxApproachActions
	};

	//近距離攻撃
	enum class CloseRangeAttack
	{
		kTackle,
		kJumpAttack,
		kComboAttack,
		kMaxCloseRangeAttacks
	};

	//遠距離攻撃
	enum class RangedAttack
	{
		kEarthSpike,
		kLaserBeam,
		kMaxRangedAttacks
	};

	//通常状態のパラメーター
	struct RootParameters
	{
		float moveSpeed = 5.0f;                 // 移動速度
		float minActionInterval = 2.0f;         // 攻撃までの最小時間
		float maxActionInterval = 4.0f;         // 攻撃までの最大時間
		float approachDistance = 10.0f;         // 近づいてくる距離
		float minWaitTimeBeforeMovement = 2.0f; // 接近前の最小待機時間
		float maxWaitTimeBeforeMovement = 4.0f; // 接近前の最大待機時間
		float stopDistance = 4.0f;              // 動きを止める距離
		float closeRangeDistance = 20.0f;       // 遠距離の行動をする距離
	};

	//走り状態のパラメーター
	struct RunTowardsPlayerParameters
	{
		float runSpeed = 20.0f; // 走り移動速度
	};

	//ダッシュ状態のパラメーター
	struct DashParameters
	{
		float dashDistance = 30.0f; // ダッシュの距離
	};

	//ジャンプ攻撃のパラメーター
	struct JumpAttackParameters
	{
		float heightOffset_ = 6.0f;
	};

	//レーザー攻撃のパラメーター
	struct LaserBeamParameters
	{
		Vector3 laserOffset = { 0.0f, 1.5f, 3.0f }; // レーザーのオフセット値
		float laserRange = 50.0f;                   // レーザーの長さ
		float easingSpeed = 10.0f;                  // イージングの速度
		float damage = 4.0f;                        // ダメージ
	};

	//柱攻撃のパラメーター
	struct EarthSpikeAttackParameters
	{
		int32_t maxPillarCount = 20;                    // 柱の数
		Vector3 pillarScale{ 2.0f, 2.0f, 2.0f };        // 柱の大きさ
		float nextPillarDelay = 0.01f;                  // 次の柱をまでの遅延時間
	};

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize() override;

    /// <summary>
    /// 更新
    /// </summary>
    void Update() override;

	/// <summary>
	/// 衝突処理
	/// </summary>
	/// <param name="gameObject">衝突相手</param>
	void OnCollision(GameObject* gameObject) override;

	/// <summary>
	/// 状態遷移
	/// </summary>
	/// <param name="newState">新しい状態</param>
	void ChangeState(IEnemyState* newState);

	//HPの取得・設定
	const float GetHP() const { return hp_; };
	void SetHP(const float hp) { hp_ = hp; };

	//タイムスケールの取得・設定
	const float GetTimeScale() const { return timeScale_; };
	void SetTimeScale(const float timeScale) { timeScale_ = timeScale; };

	//スタンから復帰した状態かどうかの取得・設定
	const bool GetIsStunRecovered() const { return isStunRecovered_; };
	void SetIsStunRecovered(const bool isStunRecovered) { isStunRecovered_ = isStunRecovered; };

	//前回の近接攻撃の取得・設定
	const CloseRangeAttack& GetPreviousCloseRangeAttack() const { return previousCloseRangeAttack_; };
	void SetPreviousCloseRangeAttack(const CloseRangeAttack& previousCloseRangeAttack) { previousCloseRangeAttack_ = previousCloseRangeAttack; };

	//前回の遠距離攻撃の取得・設定
	const RangedAttack& GetPreviousRangedAttack() const { return previousRangedAttack_; };
	void SetPreviousRangedAttack(const RangedAttack& previousRangedAttack) { previousRangedAttack_ = previousRangedAttack; };

	//デバッグフラグの取得
	const bool GetIsDebug() const { return isDebug_; };

	//各パラメータの取得
	const RootParameters& GetRootParameters() const { return rootParameters_; };
	const RunTowardsPlayerParameters& GetRunTowardsPlayerParameters() const { return runTowardsPlayerParameters_; };
	const DashParameters& GetDashParameters() const { return dashParameters_; };
	const JumpAttackParameters& GetJumpAttackParameters() const { return jumpAttackParameters_; };
	const LaserBeamParameters& GetLaserBeamParameters() const { return laserBeamParameters_; };
	const EarthSpikeAttackParameters& GetEarthSpikeAttackParameters() const { return earthSpikeAttackParameters_; };

private:
    /// <summary>
    /// アニメーターの初期化
    /// </summary>
    void InitializeAnimator() override;

    /// <summary>
    /// UIのスプライトの初期化
    /// </summary>
    void InitializeUISprites() override;

	/// <summary>
	/// スタン状態への遷移処理
	/// </summary>
	void TransitionToStunState() override;

	/// <summary>
	/// 死亡状態への遷移処理
	/// </summary>
	void TransitionToDeathState() override;

private:
	//状態
	std::unique_ptr<IEnemyState> state_ = nullptr;

	//前の近接攻撃
	CloseRangeAttack previousCloseRangeAttack_ = CloseRangeAttack::kMaxCloseRangeAttacks;

	//前の遠距離攻撃
	RangedAttack previousRangedAttack_ = RangedAttack::kMaxRangedAttacks;

	//タイムスケール
	float timeScale_ = 1.0f;

	//スタンから復帰した状態かどうか
	bool isStunRecovered_ = false;

	//各種パラメーター
	RootParameters rootParameters_{};
	RunTowardsPlayerParameters runTowardsPlayerParameters_{};
	DashParameters dashParameters_{};
	JumpAttackParameters jumpAttackParameters_{};
	LaserBeamParameters laserBeamParameters_{};
	EarthSpikeAttackParameters earthSpikeAttackParameters_{};
};

