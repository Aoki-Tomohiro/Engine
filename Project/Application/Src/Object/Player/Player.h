#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/3D/Model/AnimationManager.h"
#include "Engine/2D/Sprite.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Animator/AnimatorComponent.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GameTimer.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Application/Src/Object/Lockon/Lockon.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"
#include "Application/Src/Object/Player/SkillCooldownManager.h"
#include "Application/Src/Object/Player/States/IPlayerState.h"
#include <numbers>

class Player : public GameObject
{
public:
    //スキルの最大数
    static const int32_t kMaxSkillCount = 2;

    //ボタンの種類を示す列挙体
    enum ButtonType
    {
        A, B, X, Y, LB, RB, RT, kMaxButtons,
    };

    //スプライトの基本設定を保持する構造体
    struct SpriteSettings
    {
        std::unique_ptr<Sprite> sprite = nullptr;  // スプライト
        Vector2 position{ 0.0f, 0.0f };            // スプライトの位置
        Vector2 scale{ 1.0f, 1.0f };               // スプライトのスケール
    };

    //ボタンのUI設定を保持する構造体
    struct ButtonUISettings
    {
        SpriteSettings buttonSprite{}; // ボタンのスプライト設定
        SpriteSettings fontSprite{};   // フォントのスプライト設定
    };

    //スキルのUI設定を保持する構造体
    struct SkillUISettings
    {
        ButtonUISettings buttonSettings{};           // スキルボタンの設定
        std::unique_ptr<Sprite> cooldownBarSprite{}; // スキルクールダウンバーのスプライト
    };

    //ボタンのUI設定情報
    struct ButtonConfig
    {
        std::string buttonTexture;            // ボタンのテクスチャファイル名
        std::string fontTexture;              // フォントのテクスチャファイル名
        Vector2 buttonPosition{ 0.0f, 0.0f }; // ボタンの位置
        Vector2 fontPosition{ 0.0f, 0.0f };   // フォントの位置
        Vector2 buttonScale{ 1.0f, 1.0f };    // ボタンのスケール
        Vector2 fontScale{ 1.0f, 1.0f };      // フォントのスケール
    };

    //スキルのUI設定情報
    struct SkillConfig
    {
        ButtonConfig buttonConfig{};            // スキルボタンの設定
        Vector2 skillBarPosition{ 0.0f, 0.0f }; // スキルバーの位置
        Vector2 skillBarScale{ 1.0f, 1.0f };    // スキルバーのスケール
    };

    //アクションフラグ
    enum class ActionFlag
    {
        kDashing,                      // ダッシュ状態かどうか
        kDashAttackEnabled,            // ダッシュ攻撃状態かどうか
        kAerialAttack,                 // 空中攻撃状態かどうか
        kLaunchAttack,                 // 打ち上げ攻撃状態かどうか
        kFallingAttack,                // 落下攻撃状態かどうか
    };

    //通常状態のパラメーター
    struct RootParameters
    {
        float walkThreshold = 0.3f; // 歩きの閾値
        float walkSpeed = 9.0f;     // 歩きの移動速度
        float runThreshold = 0.6f;  // 走りの閾値
        float runSpeed = 18.0f;     // 走りの移動速度
    };

    //ジャンプ状態のパラメーター
    struct JumpParameters
    {
        float jumpFirstSpeed = 35.0f;        // 初速度
        float gravityAcceleration = -148.0f; // 重力加速度
    };

    //回避用のパラメーター
    struct DodgeParameters
    {
        float dodgeSpeed = 32.0f; // 回避速度
    };

    //ダッシュ用のパラメーター
    struct DashParameters
    {
        float dashSpeed = 125.0f;                 // ダッシュ速度
        float proximityDistance = 4.0f;           // 移動を止める距離
        float verticalAlignmentTolerance = 0.1f;  // プレイヤーと敵のY軸方向の許容誤差
    };

    //攻撃用のパラメーター
    struct AttackParameters
    {
        int32_t comboNum = 4;        // コンボの数
        float attackDistance = 6.0f; // 攻撃の補正を掛ける距離
    };

    //打ち上げ攻撃のパラメーター
    struct LaunchAttackParameters : public SkillParameters
    {
        LaunchAttackParameters() : SkillParameters{ 3.0f } {};
    };

    //回転攻撃用のパラメーター
    struct SpinAttackParameters : public SkillParameters
    {
        SpinAttackParameters() : SkillParameters{ 3.0f } {};
        float totalRotation = std::numbers::pi_v<float> *2.0f * 3.0f;
        float totalDuration = 0.6f;
        float riseHeight = 4.0f;
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
    void ChangeState(IPlayerState* newState);

    //プレイヤーの移動
    void Move(const Vector3& velocity);

    //ノックバックを適用
    void ApplyKnockback();

    //プレイヤーの回転
    void Rotate(const Vector3& vector);

    //敵の方向にプレイヤーを向かせる処理
    void LookAtEnemy();

    //アニメーションの補正
    void CorrectAnimationOffset();

    //ダメージを食らった時の処理
    void HandleIncomingDamage(const KnockbackSettings& knockbackSettings, const float damage, const bool transitionToStun);

    //アニメーション操作
    void PlayAnimation(const std::string& name, float speed, bool loop);
    void StopAnimation();
    void PauseAnimation();
    void ResumeAnimation();
    void SetIsAnimationBlending(bool isBlending);
    void SetAnimationSpeed(float animationSpeed);

    //アニメーション時間設定
    float GetCurrentAnimationTime();
    void SetCurrentAnimationTime(float animationTime);
    float GetNextAnimationTime();
    void SetNextAnimationTime(float animationTime);
    float GetCurrentAnimationDuration();
    float GetNextAnimationDuration();
    float GetCurrentAnimationSpeed();
    bool GetIsAnimationFinished();
    bool GetIsBlendingCompleted();

    //腰の位置取得
    Vector3 GetHipLocalPosition();
    Vector3 GetHipWorldPosition();

    //位置設定
    Vector3 GetPosition();
    void SetPosition(const Vector3& position);

    //クォータニオンの設定
    const Quaternion& GetDestinationQuaternion() const { return destinationQuaternion_; };
    void SetDestinationQuaternion(const Quaternion& quaternion) { destinationQuaternion_ = quaternion; };

    //アクションフラグの取得・設定
    bool GetActionFlag(const ActionFlag& flag) const;
    void SetActionFlag(const ActionFlag& flag, bool value) { flags_[flag] = value; };

    //スキルのクールダウン確認・リセット
    const bool GetIsCooldownComplete(const Skill& skill) const;
    void ResetCooldown(const Skill& skill);

    //アニメーション補正フラグの設定
    void SetIsAnimationCorrectionActive(bool isActive) { isAnimationCorrectionActive_ = isActive; };

    //死亡フラグの取得
    const bool GetIsDead() const { return isDead_; };

    //タイトルシーンのフラグ設定
    void SetIsInTitleScene(bool isInTitleScene) { isInTitleScene_ = isInTitleScene; };

    //ゲームの終了状態の取得・設定
    const bool GetIsGameFinished() const { return isGameFinished_; };
    void SetIsGameFinished(bool isGameFinished) { isGameFinished_ = isGameFinished; };

    //パーティクルエミッターの追加・削除
    void AddParticleEmitter(const std::string& name, ParticleEmitter* emitter);
    void RemoveParticleEmitter(const std::string& name, const std::string& emitterName);

    //加速フィールドの追加・削除
    void AddAccelerationField(const std::string& name, AccelerationField* field);
    void RemoveAccelerationField(const std::string& name, const std::string& fieldName);

    //パーティクルシステムの取得
    ParticleSystem* GetParticleSystem(const std::string& name) const;

    //カメラの取得・設定
    const Camera* GetCamera() const { return camera_; };
    void SetCamera(const Camera* camera) { camera_ = camera; };

    //ロックオンの取得・設定
    const Lockon* GetLockon() const { return lockon_; };
    void SetLockon(const Lockon* lockon) { lockon_ = lockon; };

    //コンバットアニメーションエディタの取得・設定
    const CombatAnimationEditor* GetCombatAnimationEditor() const { return combatAnimationEditor_; };
    void SetCombatAnimationEditor(const CombatAnimationEditor* editor) { combatAnimationEditor_ = editor; };

    //各パラメーターの取得
    const RootParameters& GetRootParameters() const { return rootParameters_; };
    const JumpParameters& GetJumpParameters() const { return jumpParameters_; };
    const DodgeParameters& GetDodgeParameters() const { return dodgeParameters_; };
    const DashParameters& GetDashParameters() const { return dashParameters_; };
    const AttackParameters& GetGroundAttackParameters() const { return groundAttackParameters_; };
    const AttackParameters& GetAerialAttackParameters() const { return aerialAttackAttackParameters_; };
    const LaunchAttackParameters& GetLaunchAttackParameters() const { return launchAttackParameters_; };
    const SpinAttackParameters& GetSpinAttackParameters() const { return spinAttackParameters_; };

private:
    //初期化関連
    void InitializeInstances();
    void InitializeState();
    void InitializeTransformComponent();
    void InitializeModelComponent();
    void InitializeAnimatorComponent();
    void InitializeColliderComponent();

    //スキル関連
    void InitializeSkillCooldownManager();
    void UpdateSkillCooldowns();
    void UpdateCooldownBarScale(SkillUISettings& uiSettings, const SkillConfig& config, float cooldownTime, float cooldownDuration);

    //スプライトとUIの設定
    void InitializeUISprites();
    void SetButtonUISprite(ButtonUISettings& uiSettings, const ButtonConfig& config);
    void SetSkillUISprite(SkillUISettings& uiSettings, const SkillConfig& config);
    void UpdateHealthBar();
    void UpdateHpBarMiddleSegment(int hpBarIndex, int middleSegmentIndex);
    void UpdateHpBarRightSegmentPosition(int hpBarIndex, int middleSegmentIndex, int rightSegmentIndex);
    void DrawSkillUI(const SkillUISettings& uiSettings);
    void DrawButtonUI(const ButtonUISettings& uiSettings);

    //プレイヤーの状態更新
    void UpdateRotation();
    void UpdateCollider();
    void RestrictPlayerMovement(float moveLimit);
    void UpdateHP();

    //エフェクト関連
    void InitializeParticleSystems();
    void UpdateVignetteEffects();
    void InitializeDamageEffect();
    void UpdateDamageEffect();  

    //ゲーム状態管理
    void CheckAndTransitionToDeath();

    //グローバル変数管理
    void ConfigureGlobalVariables();
    void ApplyGlobalVariables();

    //デバッグ関連
    void DebugUpdate();

    //ImGui関連
    void UpdateImGui();

    //音声データのロード
    void LoadAudioData();

private:
	//インプット関連
	Input* input_ = nullptr;

    //トランスフォーム関連
    TransformComponent* transform_ = nullptr;
    Vector3 preAnimationHipPosition_{};
    Vector3 colliderOffset_{};
    Quaternion destinationQuaternion_ = Mathf::IdentityQuaternion();
    float quaternionInterpolationSpeed_ = 0.4f;

    //モデル関連
    ModelComponent* model_ = nullptr;

    //アニメーション関連
    AnimatorComponent* animator_ = nullptr;

    //コライダー関連
    AABBCollider* collider_ = nullptr;

	//オーディオ関連
	Audio* audio_ = nullptr;
	uint32_t damageAudioHandle_ = 0;

	//プレイヤーの状態
	std::unique_ptr<IPlayerState> state_ = nullptr;
	std::unordered_map<ActionFlag, bool> flags_{};
	bool isAnimationCorrectionActive_ = false;
	bool isDead_ = false;
	bool isGameFinished_ = false;
	bool isInTitleScene_ = false;

	//スキル関連
	std::unique_ptr<SkillCooldownManager> skillCooldownManager_ = nullptr;
	std::array<SkillUISettings, kMaxSkillCount> skillUISettings_{};
	const std::vector<SkillConfig> skillConfigs = {
		{ "xbox_button_x_outline.png", "LaunchAttack.png", { 952.0f, 582.0f }, { 790.0f,596.0f }, {1.0f, 1.0f}, {0.3f, 0.3f}, { 955.0f, 580.0f }, { 28.0f,4.0f }},
		{ "xbox_button_y_outline.png", "SpinAttack.png", { 1000.0f,534.0f }, { 880.0f,544.0f }, {1.0f, 1.0f}, {0.3f, 0.3f} ,{ 1004.0f,530.0f }, { 28.0f,4.0f }},
	};

	//ノックバック関連
	KnockbackSettings knockbackSettings_{};

	//パーティクル関連
	std::map<std::string, ParticleSystem*> particleSystems_{};

	//カメラ関連
	const Camera* camera_ = nullptr;

	//ロックオン関連
	const Lockon* lockon_ = nullptr;

	//コンバットアニメーションエディタ関連
	const CombatAnimationEditor* combatAnimationEditor_ = nullptr;

	//体力関連
	const float kMaxHp = 100.0f;
	float hp_ = kMaxHp;
    std::array<std::array<std::unique_ptr<Sprite>, 3>, 2> hpBarSegments_{};
    std::vector<std::vector<Vector2>> hpBarSegmentPositions_{
        {{71.0f, 40.0f}, {80.0f, 40.0f}, {560.0f, 40.0f}},
        {{71.0f, 40.0f}, {80.0f, 40.0f}, {560.0f, 40.0f}},
    };
    Vector2 hpBarSegmentTextureSize_ = { 480.0f,18.0f };

	//デバッグ関連
	bool isDebug_ = false;
	std::string currentAnimationName_ = "Idle";
	float animationTime_ = 0.0f;
	std::vector<std::string> animationName_ = {
		"Idle", "Walk1", "Walk2", "Walk3", "Walk4", "Run1", "Run2", "Run3", "Run4", "Jump1", "Jump2", "Dodge1", "Dodge2", "Dodge3", "Dodge4",
		"DashStart", "DashEnd", "Falling", "GroundAttack1", "GroundAttack2", "GroundAttack3", "GroundAttack4", "AerialAttack1", "AerialAttack2", "AerialAttack3",
		"LaunchAttack", "SpinAttack", "FallingAttack", "DodgeForward"
	};

	//UI関連
	std::array<ButtonUISettings, kMaxButtons> buttonUISettings_{};
	const std::vector<ButtonConfig> buttonConfigs = {
		{ "xbox_button_a_outline.png", "Jump.png", {1000.0f, 630.0f}, {1060.0f, 644.0f}, {1.0f, 1.0f}, {0.3f, 0.3f} },
		{ "xbox_button_b_outline.png", "Dash.png", {1048.0f, 582.0f}, {1108.0f, 596.0f}, {1.0f, 1.0f}, {0.3f, 0.3f} },
		{ "xbox_button_x_outline.png", "Attack.png", {952.0f, 582.0f}, {880.0f, 596.0f}, {1.0f, 1.0f}, {0.3f, 0.3f} },
		{ "xbox_button_y_outline.png", "Fire.png", {1000.0f, 534.0f}, {904.0f, 544.0f}, {1.0f, 1.0f}, {0.3f, 0.3f} },
		{ "xbox_lb_outline.png", "Lockon.png", {1070.0f, 429.0f}, {1139.0f, 439.0f}, {1.0f, 1.0f}, {0.3f, 0.3f} },
		{ "xbox_rb_outline.png", "Dodge.png", {1070.0f, 484.0f}, {1139.0f, 496.0f}, {1.0f, 1.0f}, {0.3f, 0.3f} },
		{ "xbox_rt_outline.png", "Change.png", {1070.0f, 370.0f}, {1139.0f, 382.0f}, {0.5f, 0.5f}, {0.3f, 0.3f} }
	};

    //ダメージエフェクト関連
    std::unique_ptr<Sprite> damageEffectSprite_ = nullptr;
    Vector4 damageEffectColor_{ 1.0f,0.0f,0.0f,0.0f };
    float damageEffectTimer_ = 0.0f;
    float damageEffectDuration_ = 1.0f;

	//各種パラメーター
	RootParameters rootParameters_{};
	JumpParameters jumpParameters_{};
	DodgeParameters dodgeParameters_{};
	DashParameters dashParameters_{};
	AttackParameters groundAttackParameters_{ .comboNum = 4, .attackDistance = 3.0f };
	AttackParameters aerialAttackAttackParameters_{ .comboNum = 3, .attackDistance = 3.0f };
	LaunchAttackParameters launchAttackParameters_{};
	SpinAttackParameters spinAttackParameters_{};
};

