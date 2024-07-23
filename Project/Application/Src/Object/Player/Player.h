#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/3D/Camera/LockOn.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "States/IPlayerState.h"

class Player : public GameObject
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void DrawUI() override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	void SetCamera(const Camera* camera) { camera_ = camera; };

	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }

	void SetIsInTitleScene(const bool isInTitleScene) { isInTitleScene_ = isInTitleScene; };

	const bool GetIsJustDodgeSuccess() const { return isJustDodgeSuccess_; };

	const bool GetIsJustDodgeAttack() const { return isJustDodgeAttack_; };

	const bool GetIsAirAttack() const { return isAirAttack_; };

private:
	//通常状態の構造体
	struct RootParameters
	{
		float walkThreshold = 0.3f;               // 歩きのスティック入力の閾値
		float walkSpeed = 9.0f;                   // 歩きの移動速度
		float runThreshold = 0.6f;                // 走りのスティック入力の閾値
		float runSpeed = 18.0f;                   // 走りの移動速度
	};

	//ダッシュ用の構造体
	struct DashParameters
	{
		float proximityThreshold = 10.0f; // 移動を止める距離
		float dashSpeed_ = 96.0f;         // ダッシュの移動速度
		float dashDuration = 0.5f;        // ダッシュ時間
		float chargeDuration = 0.16f;     // チャージ時間
	};

	//回避用の構造体
	struct DodgeParameters
	{
		float dodgeSpeed = 18.0f;               // 回避の移動速度
		float justDodgeSuccessDistance = 10.0f; // ジャスト回避が成功可能距離
		float justDodgeSuccessDuration = 1.0f;  // ジャスト回避の成功可能時間
		float justDodgeDuration = 1.2f;         // ジャスト回避の時間
		float justDodgeTargetDistance = 20.0f;  // ジャスト回避成功時の移動距離
	};

	//ジャンプの構造体
	struct JumpParameters
	{
		float firstSpeed = 45.0f;         // ジャンプの初速度
		float gravityAcceleration = 2.8f; // ジャンプの重力加速度
	};

	//地上攻撃用の構造体
	struct GroundAttackParameters
	{
		float attackDistance = 10.0f;  // 攻撃の補正を掛ける距離
		float parryDuration = 0.1f;    // パリィの成功時間
	};

	//空中攻撃用の構造体
	struct AirAttackParameters
	{
		float attackDistance = 11.0f;            // 攻撃の補正を掛ける距離
		float parryDuration = 0.1f;              // パリィの成功時間
		float gravityAcceleration = 2.8f;        // 重力加速度
		float attackGravityAcceleration_ = 0.1f; // 攻撃中の重力加速度
		float verticalKnockback = 10.0f;         // 攻撃が当たった時の上昇力
	};

	//回避攻撃用の構造体
	struct JustDodgeAttackParameters
	{
		float moveDuration = 0.4f;    // 移動時間
		float targetDistance = -8.0f; // 移動後の敵との距離
		float hitInterval = 0.15f;    // ヒット間隔
		int32_t maxHitCount = 1;      // ヒット数
	};

	//魔法攻撃用の構造体
	struct MagicAtackParameters
	{
		float fireRate = 0.2f;                             // 魔法攻撃の発射間隔
		float magicAttackFinishedDuration = 1.0f;          // 魔法攻撃が終了するまでの時間
		Vector3 magicProjectileScale = { 0.6f,0.6f,0.6f }; // 魔法弾の大きさ
		float magicProjectileSpeed = 96.0f;                // 魔法弾の速度
		float enhancedMagicProjectileFireRate = 0.92f;     // 強化魔法の発射間隔
		float enhancedMagicProjectileSpeed = 48.0f;        // 強化魔法弾の速度
		float enhancedMagicWindow = 0.06f;                 // 強化魔法弾の受付時間
		float chargeMagicInputDuration = 2.0f;             // チャージ魔法の入力時間
	};

	void ChangeState(IPlayerState* state);

	void AddMagicProjectile(const bool isEnhanced);

	void UpdateChargeMagicProjectile();

	void ApplyGlobalVariables();

private:
	//Input
	Input* input_ = nullptr;

	//プレイヤーの状態
	std::unique_ptr<IPlayerState> state_ = nullptr;

	//速度
	Vector3 velocity{};

	//Quaternion
	Quaternion destinationQuaternion_{ 0.0f,0.0f,0.0f,1.0f };

	//回転の保管速度
	float quaternionInterpolationSpeed_ = 0.4f;

	//Camera
	const Camera* camera_ = nullptr;

	//LockOn
	const LockOn* lockOn_ = nullptr;

	//タイトルシーンなのか
	bool isInTitleScene_ = false;

	//通常状態用のパラメーター
	RootParameters rootParameters_{};

	//ダッシュ用のパラメーター
	DashParameters dashParameters_{};

	//回避用のパラメーター
	DodgeParameters dodgeParameters_{}; 

	//ジャンプ用のパラメーター
	JumpParameters jumpParameters_{};

	//地上攻撃用のパラメーター
	GroundAttackParameters groundAttackParameters_{};

	//空中攻撃用のパラメーター
	AirAttackParameters airAttackParameters_{};

	//回避攻撃用のパラメーター
	JustDodgeAttackParameters justDodgeAttackParameters_{};

	//魔法攻撃用のパラメーター
	MagicAtackParameters magicAttackParameters_{};

	//ジャスト回避に成功したかどうか
	bool isJustDodgeSuccess_ = false;

	//回避攻撃に派生したかどうか
	bool isJustDodgeAttack_ = false;

	//ダッシュ攻撃に派生したかどうか
	bool isDashAttack_ = false;

	//空中攻撃中かどうか
	bool isAirAttack_ = false;

	//デバッグ用のフラグ
	bool isDebug_ = false;

	//チャージ魔法が終了しているか
	bool isChargeMagicFinished_ = false;

	//チャージ魔法用のタイマー
	float chargeMagicTimer_ = 0.0f;

	//パーティクル
	ParticleSystem* chargeMagicParticle_ = nullptr;

	//アニメーションの時間
	float animationTime_ = 0.0f;

	//アニメーション一覧
	std::vector<std::string> animationName_ = {
		{"Armature|mixamo.com|Layer0"},
		{"Armature.001|mixamo.com|Layer0"},
		{"Armature.001|mixamo.com|Layer0.001"},
		{"Armature.001|mixamo.com|Layer0.002"},
		{"Armature.001|mixamo.com|Layer0.003"},
		{"Armature.001|mixamo.com|Layer0.004"},
		{"Armature.001|mixamo.com|Layer0.005"},
		{"Armature.001|mixamo.com|Layer0.006"},
		{"Armature.001|mixamo.com|Layer0.007"},
		{"Armature.001|mixamo.com|Layer0.008"},
		{"Armature.001|mixamo.com|Layer0.009"},
		{"Armature.001|mixamo.com|Layer0.010"},
		{"Armature.001|mixamo.com|Layer0.011"},
		{"Armature.001|mixamo.com|Layer0.012"},
		{"Armature.001|mixamo.com|Layer0.013"},
		{"Armature.001|mixamo.com|Layer0.014"},
		{"Armature.001|mixamo.com|Layer0.015"},
		{"Armature.001|mixamo.com|Layer0.016"},
		{"Armature.001|mixamo.com|Layer0.017"},
		{"Armature.001|mixamo.com|Layer0.018"},
		{"Armature.001|mixamo.com|Layer0.019"},
		{"Armature.001|mixamo.com|Layer0.020"},
		{"Armature.001|mixamo.com|Layer0.021"},
		{"Armature.001|mixamo.com|Layer0.022"},
		{"Armature.001|mixamo.com|Layer0.023"},
		{"Armature.001|mixamo.com|Layer0.024"},
		{"Armature.001|mixamo.com|Layer0.025"},
		{"Armature.001|mixamo.com|Layer0.026"},
		{"Armature.001|mixamo.com|Layer0.027"},
		{"Armature.001|mixamo.com|Layer0.028"},
	};

	//現在のアニメーション
	std::string currentAnimationName_ = animationName_[0];

	//フレンドクラスに登録
	friend class PlayerStateRoot;
	friend class PlayerStateDodge;
	friend class PlayerStateJustDodge;
	friend class PlayerStateJustDodgeAttack;
	friend class PlayerStateJump;
	friend class PlayerStateGroundAttack;
	friend class PlayerStateAirAttack;
	friend class PlayerStateDash;
};

