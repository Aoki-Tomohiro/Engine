#pragma once
#include "Engine/3D/Model/AnimationManager.h"
#include "Application/Src/Object/Character/BaseCharacter.h"

/// <summary>
/// 敵
/// </summary>
class Enemy : public BaseCharacter
{
public:
	//レベル
	enum Level
	{
		Easy,
		Normal,
		Hard,
		Count
	};

	//攻撃カテゴリ
	enum class AttackCategory
	{
		CloseRange,  //近接攻撃
		Ranged,      //遠距離攻撃
		Count        //カテゴリの総数
	};

	//距離を詰めるタイプ
	enum class ApproachType
	{
		DashForward,      //突進
		RunTowardsPlayer, //プレイヤーに向かって走る
		MaxTypes          //アプローチタイプの最大値
	};

	//近距離攻撃タイプ
	enum class AttackType
	{
		VerticalSlash, //垂直斬り
		ComboSlash,    //連続斬り
		SpinSlash,     //回転斬り
		MaxTypes       //タイプの最大値
	};

	//アクションフラグ
	enum class ActionFlag
	{
		kCanAttack,   //攻撃できる状態かどうか
		kCanDodge,    //回避できる状態かどうか
	};

	//通常状態のパラメーター
	struct RootParameters
	{
		float dodgeStartTime = 0.6f;            //攻撃をよける行動をとり始める時間
		float minActionInterval = 1.4f;         //攻撃までの最小時間
		float maxActionInterval = 2.0f;         //攻撃までの最大時間
		float minWaitTimeBeforeMovement = 1.0f; //接近前の最小待機時間
		float maxWaitTimeBeforeMovement = 2.0f; //接近前の最大待機時間
		float closeRangeAttackDistance = 6.0f;  //近接攻撃する距離
		float approachDistance = 10.0f;         //前進する距離
		float rangedAttackDistance = 14.0f;     //遠距離攻撃する距離
	};

	//攻撃状態のパラメーター
	struct AttackParameters
	{
		float justDodgeTime = 0.2f;
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
	/// プレイヤーの方向に敵を向かせる処理
	/// </summary>
	void LookAtPlayer();

	/// <summary>
	/// 攻撃状態のリセット
	/// </summary>
	void ResetAttackState();

	/// <summary>
	/// プレイヤーと敵の座標を取得して距離を計算する関数
	/// </summary>
	/// <returns>プレイヤーとの距離</returns>
	float CalculateDistanceToPlayer() const;

	//HPの取得・設定
	const float GetHP() const { return hp_; };
	void SetHP(const float hp) { hp_ = hp; };

	//アクションフラグの取得・設定
	const bool GetActionFlag(const ActionFlag& actionFlag) const { auto it = actionFlags_.find(actionFlag); return it != actionFlags_.end() && it->second; };
	void SetActionFlag(const ActionFlag& actionFlag, bool value) { actionFlags_[actionFlag] = value; };

	//前回の攻撃の取得・設定
	const AttackType& GetPreviousAttack() const { return previousCloseRangeAttack_; };
	void SetPreviousAttack(const AttackType& previousCloseRangeAttack) { previousCloseRangeAttack_ = previousCloseRangeAttack; };

	//難易度の取得・設定
	const Level GetLevel() const { return currentLevel_; };
	void SetLevel(const Enemy::Level level) { currentLevel_ = level; };

	//プレイヤーとの距離を取得
	const float GetDistanceToPlayer() const { return distanceToPlayer_; };

	//各パラメータの取得
	const RootParameters& GetRootParameters() const { return rootParameters_[currentLevel_]; };
	const AttackParameters& GetAttackParameters() const { return attackParameters_[currentLevel_]; };

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

	/// <summary>
	/// 攻撃タイマーの更新
	/// </summary>
	void UpdateAttackTimer();

	/// <summary>
	/// 環境変数の初期化
	/// </summary>
	void InitializeGlobalVariables() override;

	/// <summary>
	/// 環境変数の適用
	/// </summary>
	void ApplyGlobalVariables() override;

private:
	//現在のレベル
	static Level currentLevel_;

	//現在の攻撃間隔
	float attackInterval_ = 0.0f;

	//攻撃間隔のタイマー
	float elapsedAttackTime_ = 0.0f;

	//プレイヤーとの距離
	float distanceToPlayer_ = 0.0f;

	//前の近接攻撃
	AttackType previousCloseRangeAttack_ = AttackType::MaxTypes;

	//アクションフラグ
	std::unordered_map<ActionFlag, bool> actionFlags_{};

	//各種パラメーター
	std::array<RootParameters, Level::Count> rootParameters_ = { {
		{4.0f, 4.0f, 6.0f, 1.0f, 2.0f, 6.0f, 10.0f, 14.0f },
		{2.0f, 2.0f, 4.0f, 1.0f, 2.0f, 6.0f, 10.0f, 14.0f },
		{0.6f, 1.4f, 2.0f, 1.0f, 2.0f, 6.0f, 10.0f, 14.0f },}
	};
	std::array<AttackParameters, Level::Count> attackParameters_ = { {
		{ 0.3f }, { 0.2f }, { 0.1f } }
	};
};

