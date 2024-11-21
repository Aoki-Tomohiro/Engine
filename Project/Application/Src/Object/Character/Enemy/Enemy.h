#pragma once
#include "Engine/3D/Model/AnimationManager.h"
#include "Application/Src/Object/Character/BaseCharacter.h"

/// <summary>
/// 敵
/// </summary>
class Enemy : public BaseCharacter
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

	//アクションフラグ
	enum class ActionFlag
	{
		kCanAttack, //攻撃できる状態かどうか

	};

	//通常状態のパラメーター
	struct RootParameters
	{
		float moveSpeed = 5.0f;                 // 移動速度
		float minActionInterval = 1.0f;         // 攻撃までの最小時間
		float maxActionInterval = 2.0f;         // 攻撃までの最大時間
		float approachDistance = 10.0f;         // 近づいてくる距離
		float minWaitTimeBeforeMovement = 1.0f; // 接近前の最小待機時間
		float maxWaitTimeBeforeMovement = 2.0f; // 接近前の最大待機時間
		float closeRangeDistance = 20.0f;       // 遠距離の行動をする距離
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

	//HPの取得・設定
	const float GetHP() const { return hp_; };
	void SetHP(const float hp) { hp_ = hp; };

	//タイムスケールの取得・設定
	const float GetTimeScale() const { return timeScale_; };
	void SetTimeScale(const float timeScale) { timeScale_ = timeScale; };

	//各パラメータの取得
	const RootParameters& GetRootParameters() const { return rootParameters_; };

private:
	/// <summary>
	/// アクションマップの初期化
	/// </summary>
	void InitializeActionMap() override;

	/// <summary>
	/// オーディオの初期化
	/// </summary>
	void InitializeAudio() override;

    /// <summary>
    /// アニメーターの初期化
    /// </summary>
    void InitializeAnimator() override;

	/// <summary>
	/// UIのスプライトの初期化
	/// </summary>
	void InitializeUISprites() override;

private:
	//タイムスケール
	float timeScale_ = 1.0f;

	//各種パラメーター
	RootParameters rootParameters_{};
};

