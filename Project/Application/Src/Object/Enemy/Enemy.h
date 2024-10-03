#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/2D/Sprite.h"
#include "Engine/3D/Model/AnimationManager.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Components/Animator/AnimatorComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GameTimer.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Application/Src/Object/Enemy/States/IEnemyState.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"
#include <numbers>

class Enemy : public GameObject
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
		float dashDistance = 20.0f; // ダッシュの距離
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

	//初期化処理
	void Initialize() override;

	//更新処理
	void Update() override;

	//描画処理
	void Draw(const Camera& camera) override;
	void DrawUI() override;

	//衝突処理
	void OnCollision(GameObject* gameObject) override;
	void OnCollisionEnter(GameObject* gameObject) override;
	void OnCollisionExit(GameObject* gameObject) override;

	//リセット処理
	void Reset() override;

	//状態遷移
	void ChangeState(IEnemyState* newState);

	//移動処理
	void Move(const Vector3& velocity);

	//回転処理
	void Rotate(const Vector3& vector);

	//武器にパラメーターを設定
	void ApplyParametersToWeapon(const CombatPhase& combatPhase);

	//ノックバックを適用
	void ApplyKnockback();

	//モデルシェイク開始
	void StartModelShake();

	//アニメーションの補正
	void CorrectAnimationOffset();

	//ダメージを食らった時の処理
	void ProcessCollisionImpact(GameObject* gameObject, const bool transitionToStun);
	void ApplyDamageAndKnockback(const KnockbackSettings& knockbackSettings, const float damage, const bool transitionToStun);

	//アニメーション操作
	void PlayAnimation(const std::string& name, const float speed, const bool loop);
	void StopAnimation();
	void SetIsAnimationBlending(bool isBlending);
	void SetBlendDuration(const float blendDuration);

	//アニメーション時間設定
	float GetCurrentAnimationTime();
	float GetNextAnimationTime();
	float GetCurrentAnimationDuration();
	float GetNextAnimationDuration();
	const bool GetIsAnimationFinished();
	const bool GetIsBlendingCompleted();

	//ジョイントの位置取得
	const Vector3 GetJointWorldPosition(const std::string& jointName);
	const Vector3 GetHipLocalPosition();
	const Vector3 GetHipWorldPosition();

	//位置設定
	const Vector3 GetPosition();
	void SetPosition(const Vector3& position);

	//クォータニオンの設定
	const Quaternion& GetQuaternion() const { return transform_->worldTransform_.quaternion_; };
	const Quaternion& GetDestinationQuaternion() const { return destinationQuaternion_; };
	void SetDestinationQuaternion(const Quaternion& destinationQuaternion) { destinationQuaternion_ = destinationQuaternion; };

	//タイムスケールの設定
	const float GetTimeScale() const { return timeScale_; };
	void SetTimeScale(const float timeScale) { timeScale_ = timeScale; };

	//HPの設定
	const float GetHP() const { return hp_; };
	void SetHP(const float hp) { hp_ = hp; };

	//アニメーション補正フラグの設定
	void SetIsAnimationCorrectionActive(const float isAnimationCorrectionActive) { isAnimationCorrectionActive_ = isAnimationCorrectionActive; };

	//回転の補間速度を設定
	void SetQuaternionInterpolationSpeed(const float quaternionInterpolationSpeed) { quaternionInterpolationSpeed_ = quaternionInterpolationSpeed; };

	//死亡フラグの取得
	const bool GetIsDead() const { return isDead_; };

	//デバッグフラグの取得
	const bool GetIsDebug() const { return isDebug_; };

	//タイトルシーンのフラグを設定
	void SetIsInTitleScene(const bool isInTitleScene) { isInTitleScene_ = isInTitleScene; };

	//ゲームの終了状態の取得・設定
	const bool GetIsGameFinished() const { return isGameFinished_; };
	void SetIsGameFinished(const bool isGameFinished) { isGameFinished_ = isGameFinished; };

	//スタンから復帰した状態かどうかの取得・設定
	const bool GetIsStunRecovered() const { return isStunRecovered_; };
	void SetIsStunRecovered(const bool isStunRecovered) { isStunRecovered_ = isStunRecovered; };

	//前回の近接攻撃の取得・設定
	const CloseRangeAttack& GetPreviousCloseRangeAttack() const { return previousCloseRangeAttack_; };
	void SetPreviousCloseRangeAttack(const CloseRangeAttack& previousCloseRangeAttack) { previousCloseRangeAttack_ = previousCloseRangeAttack; };

	//前回の遠距離攻撃の取得・設定
	const RangedAttack& GetPreviousRangedAttack() const { return previousRangedAttack_; };
	void SetPreviousRangedAttack(const RangedAttack& previousRangedAttack) { previousRangedAttack_ = previousRangedAttack; };

	//ノックバックを取得・設定
	const KnockbackSettings& GetKnockbackSettings() const { return knockbackSettings_; };
	void SetKnockbackSettings(const KnockbackSettings& knockbackSettings) { knockbackSettings_ = knockbackSettings; };

	//コンバットアニメーションエディタの取得・設定
	const CombatAnimationEditor* GetCombatAnimationEditor() const { return combatAnimationEditor_; };
	void SetCombatAnimationEditor(const CombatAnimationEditor* combatAnimationEditor) { combatAnimationEditor_ = combatAnimationEditor; };

	//各パラメータの取得
	const RootParameters& GetRootParameters() const { return rootParameters_; };
	const RunTowardsPlayerParameters& GetRunTowardsPlayerParameters() const { return runTowardsPlayerParameters_; };
	const DashParameters& GetDashParameters() const { return dashParameters_; };
	const JumpAttackParameters& GetJumpAttackParameters() const { return jumpAttackParameters_; };
	const LaserBeamParameters& GetLaserBeamParameters() const { return laserBeamParameters_; };
	const EarthSpikeAttackParameters& GetEarthSpikeAttackParameters() const { return earthSpikeAttackParameters_; };

private:
	//初期化関連
	void InitializeState();
	void InitializeTransformComponent();
	void InitializeModelComponent();
	void InitializeAnimatorComponent();
	void InitializeColliderComponent();

	//UIの設定
	void InitializeHPSprites();
	void UpdateHealthBar();
	void UpdateHpBarMiddleSegment(int hpBarIndex, int middleSegmentIndex);
	void UpdateHpBarRightSegmentPosition(int hpBarIndex, int middleSegmentIndex, int rightSegmentIndex);

	//敵の状態更新
	void UpdateRotation();
	void UpdateCollider();
	void RestrictEnemyMovement(float moveLimit);
	void UpdateHP();
	void ResetToOriginalPosition();
	void UpdateModelShake();
	void ApplyModelShake();

	//デバッグ関連
	void DebugUpdate();

	//グローバル変数管理
	void ConfigureGlobalVariables();
	void ApplyGlobalVariables();

	//ImGui関連
	void UpdateImGui();

	//ゲーム状態管理
	void CheckAndTransitionToDeath();

private:
	//トランスフォーム関連
	TransformComponent* transform_ = nullptr;
	Quaternion destinationQuaternion_ = Mathf::IdentityQuaternion();
	float quaternionInterpolationSpeed_ = 0.4f;
	Vector3 preAnimationHipPosition_{};
	Vector3 colliderOffset_{};

	//モデル関連
	ModelComponent* model_ = nullptr;

	//アニメーション関連
	AnimatorComponent* animator_ = nullptr;

	//コライダー関連
	AABBCollider* collider_ = nullptr;

	//敵の状態
	std::unique_ptr<IEnemyState> state_ = nullptr;
	CloseRangeAttack previousCloseRangeAttack_ = CloseRangeAttack::kMaxCloseRangeAttacks;
	RangedAttack previousRangedAttack_ = RangedAttack::kMaxRangedAttacks;
	bool isAnimationCorrectionActive_ = false;
	bool isDead_ = false;
	bool isGameFinished_ = false;
	bool isInTitleScene_ = false;
	bool isStunRecovered_ = false;

	//モデルシェイク用の変数
	bool isModelShakeActive_ = false;
	float modelShakeDuration_ = 0.1f;
	float elapsedModelShakeTime_ = 0.0f;
	Vector3 originalModelPosition_{};
	Vector3 modelShakeIntensity_{ 40.0f,0.0f,40.0f };

	//重力加速度
	float gravityAcceleration = -42.0f;

	//タイムスケール
	float timeScale_ = 1.0f;

	//ノックバック関連
	KnockbackSettings knockbackSettings_{};

	//コンバットアニメーションエディタ関連
	const CombatAnimationEditor* combatAnimationEditor_ = nullptr;

	//体力関連
	const float kMaxHp = 800.0f;
	float hp_ = kMaxHp;
	std::array<std::array<std::unique_ptr<Sprite>, 3>, 2> hpBarSegments_{};
	std::vector<std::vector<Vector2>> hpBarSegmentPositions_{
		{{711.0f, 40.0f}, {720.0f, 40.0f}, {1200.0f, 40.0f}},
		{{711.0f, 40.0f}, {720.0f, 40.0f}, {1200.0f, 40.0f}},
	};
	Vector2 hpBarSegmentTextureSize_ = { 480.0f,18.0f };

	//デバッグ関連
	bool isDebug_ = false;
	float animationTime_ = 0.0f;
	std::string currentAnimationName_ = "Idle";
	std::vector<std::string> animationName_ = {
		{"Idle"}, {"Walk1"}, {"Walk2"}, {"Walk3"}, {"Walk4"}, {"Run"}, {"Dash"}, {"TackleAttack"}, {"JumpAttack"}, {"ComboAttack"}, {"EarthSpike"}, {"LaserBeam"},
		{"HitStun"}, {"Knockdown"}, {"StandUp"}, {"Death"},
	};

	//各種パラメーター
	RootParameters rootParameters_{};
	RunTowardsPlayerParameters runTowardsPlayerParameters_{};
	DashParameters dashParameters_{};
	JumpAttackParameters jumpAttackParameters_{};
	LaserBeamParameters laserBeamParameters_{};
	EarthSpikeAttackParameters earthSpikeAttackParameters_{};
};

