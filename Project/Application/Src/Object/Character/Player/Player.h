#pragma once
#include "Engine/3D/Model/AnimationManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Application/Src/Object/Character/BaseCharacter.h"
#include "Application/Src/Object/Character/States/PlayerStates/IPlayerState.h"
#include "Application/Src/Object/Lockon/Lockon.h"
#include "Application/Src/Object/Character/Player/SkillCooldownManager.h"
#include <numbers>

/// <summary>
/// プレイヤー
/// </summary>
class Player : public BaseCharacter
{
public:
    //スキルの最大数
    static const int32_t kMaxSkillCount = 2;

    //ボタンの種類を示す列挙体
    enum ButtonType
    {
        A, B, X, Y, LB, RB, RT, kMaxButtons,
    };

    //ボタンの入力状態
    struct ButtonState
    {
        bool isPressed = false;   //押されているかどうか
        bool isTriggered = false; //押された瞬間かどうか
        bool isReleased = false;  //離された瞬間かどうか
        int32_t pressedFrame = 0; //押されているフレーム
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
        kMagicAttackEnabled,           // 魔法攻撃が有効かどうか
        kChargeMagicAttackEnabled,     // 溜め魔法攻撃が有効かどうか
        kCanStomp,                     // ストンプが可能かどうか
    };

    //ダメージエフェクトの構造体
    struct DamageEffect
    {
        std::unique_ptr<Sprite> sprite = nullptr; // スプライト
        Vector4 color{ 1.0f, 0.0f, 0.0f, 0.0f };  // 色
        float timer = 0.0f;                       // タイマー
        float duration = 1.0f;                    // 持続時間
    };

    //魔法攻撃用ワーク
    struct MagicAttackWork
    {
        bool isCharging = false;    //魔法がチャージされているかどうか
        float chargeTimer = 0.0f;   //チャージ用のタイマー
        float cooldownTimer = 0.0f; //クールタイム用のタイマー
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
        float jumpFirstSpeed = 40.0f;        // 初速度
        float gravityAcceleration = -180.0f; // 重力加速度
    };

    //ダッシュ用のパラメーター
    struct DashParameters
    {
        float proximityDistance = 6.0f;           // 移動を止める距離
        float verticalAlignmentTolerance = 0.1f;  // プレイヤーと敵のY軸方向の許容誤差
    };

    //攻撃用のパラメーター
    struct AttackParameters
    {
        int32_t comboNum = 4;        // コンボの数
        float attackDistance = 6.0f; // 攻撃の補正を掛ける距離
    };

    //魔法攻撃用のパラメーター
    struct MagicAttackParameters
    {
        float chargeTimeThreshold = 1.0f;               // チャージマジックのため時間
        float cooldownTime = 0.6f;                      // 通常魔法のクールタイム
        float magicProjectileSpeed = 96.0f;             // 魔法の速度
        float verticalRetreatSpeed = 10.0f;             // 垂直後退速度
        Vector3 acceleration = { 0.0f, -64.0f ,60.0f }; // 加速度
    };

    //打ち上げ攻撃のパラメーター
    struct LaunchAttackParameters : public SkillParameters
    {
        LaunchAttackParameters() : SkillParameters{ 4.0f } {};
    };

    //回転攻撃用のパラメーター
    struct SpinAttackParameters : public SkillParameters
    {
        SpinAttackParameters() : SkillParameters{ 2.6f } {};
        float totalRotation = std::numbers::pi_v<float> *2.0f * 3.0f;
        float totalDuration = 0.6f;
        float riseHeight = 4.0f;
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
    /// UIの描画
    /// </summary>
    void DrawUI() override;

    /// <summary>
    /// 衝突処理
    /// </summary>
    /// <param name="gameObject">衝突相手</param>
    void OnCollision(GameObject* gameObject) override;

    /// <summary>
    /// 状態遷移処理
    /// </summary>
    /// <param name="newState">新しい状態</param>
    void ChangeState(IPlayerState* newState);

    /// <summary>
    /// 敵の方向にプレイヤーを向かせる処理
    /// </summary>
    void LookAtEnemy();

    /// <summary>
    /// ダメージとノックバックを適用
    /// </summary>
    /// <param name="knockbackSettings">ノックバックの設定</param>
    /// <param name="damage">ダメージ</param>
    /// <param name="transitionToStun">スタン状態に遷移するかどうか</param>
    virtual void ApplyDamageAndKnockback(const KnockbackSettings& knockbackSettings, const float damage, const bool transitionToStun);

    //単押しの判定
    bool IsTriggered(const ButtonType buttonType) { return buttonStates_[buttonType].isTriggered; };

    //離した瞬間の処理
    bool IsReleased(int buttonIndex) { return buttonStates_[buttonIndex].isReleased; };

    //同時押しの判定
    bool ArePressed(const ButtonType buttonType1, const ButtonType buttonType2) { return buttonStates_[buttonType1].isPressed && buttonStates_[buttonType2].isPressed; };

    //魔法攻撃のクールダウンをリセット
    void ResetMagicAttackCooldownTime() { magicAttackWork_.cooldownTimer = magicAttackParameters_.cooldownTime; };

    //アクションフラグの取得・設定
    const bool GetActionFlag(const ActionFlag& actionFlag) const { auto it = actionFlags_.find(actionFlag); return it != actionFlags_.end() && it->second; };
    void SetActionFlag(const ActionFlag& actionFlag, bool value) { actionFlags_[actionFlag] = value; };

    //スキルのクールダウン確認・リセット
    const bool GetIsCooldownComplete(const Skill& skill) const { return skillCooldownManager_->IsCooldownComplete(skill); };
    void ResetCooldown(const Skill& skill) { skillCooldownManager_->ResetCooldown(skill); };

    //カメラを設定・取得
    void SetCamera(const Camera* camera) { camera_ = camera; };
    const Camera* GetCamera() const { return camera_; };

	//ロックオンを設定・取得
	void SetLockon(const Lockon* lockon) { lockon_ = lockon; };
    const Lockon* GetLockon() const { return lockon_; };

    //各パラメーターの取得
    const RootParameters& GetRootParameters() const { return rootParameters_; };
    const JumpParameters& GetJumpParameters() const { return jumpParameters_; };
    const DashParameters& GetDashParameters() const { return dashParameters_; };
    const AttackParameters& GetGroundAttackParameters() const { return groundAttackParameters_; };
    const AttackParameters& GetAerialAttackParameters() const { return aerialAttackAttackParameters_; };
    const LaunchAttackParameters& GetLaunchAttackParameters() const { return launchAttackParameters_; };
    const SpinAttackParameters& GetSpinAttackParameters() const { return spinAttackParameters_; };
    const MagicAttackParameters& GetMagicAttackParameters() const { return magicAttackParameters_; };

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
    /// ボタンのUIスプライトを設定
    /// </summary>
    void SetButtonUISprite(ButtonUISettings& uiSettings, const ButtonConfig& config);

    /// <summary>
    /// スキルのUIスプライトを設定
    /// </summary>
    void SetSkillUISprite(SkillUISettings& uiSettings, const SkillConfig& config);

    /// <summary>
    /// 魔法攻撃の更新
    /// </summary>
    void UpdateMagicAttack();

    /// <summary>
    /// スキルクールダウンマネージャーの更新
    /// </summary>
    void UpdateSkillCooldowns();

    /// <summary>
    /// スキルクールダウンバーの大きさを更新
    /// </summary>
    void UpdateCooldownBarScale(SkillUISettings& uiSettings, const SkillConfig& config, float cooldownTime, float cooldownDuration);

    /// <summary>
    /// ダメージエフェクトの更新
    /// </summary>
    void UpdateDamageEffect();

    /// <summary>
    /// ボタンのUIの描画
    /// </summary>
    /// <param name="uiSettings">UIの設定</param>
    void DrawButtonUI(const ButtonUISettings& uiSettings);

    /// <summary>
    /// スキルのUIの描画
    /// </summary>
    /// <param name="uiSettings">スキルのUIの設定</param>
    void DrawSkillUI(const SkillUISettings& uiSettings);

    /// <summary>
    /// スタン状態への遷移処理
    /// </summary>
    void TransitionToStunState() override;

    /// <summary>
    /// 死亡状態への遷移処理
    /// </summary>
    void TransitionToDeathState() override;

    /// <summary>
    /// ボタンの入力状態の更新
    /// </summary>
    void UpdateButtonStates();

private:
    //インプット
    Input* input_ = nullptr;

    //オーディオ
    Audio* audio_ = nullptr;

    //状態
    std::unique_ptr<IPlayerState> state_ = nullptr;

    //アクションフラグ
    std::unordered_map<ActionFlag, bool> actionFlags_{};

    //スキルクールダウンマネージャー
    std::unique_ptr<SkillCooldownManager> skillCooldownManager_ = nullptr;

    //ボタンの状態
    std::array<ButtonState, kMaxButtons> buttonStates_{};

    //ボタン定数を保持する配列
    std::array<WORD, kMaxButtons> buttonMappings_{
        XINPUT_GAMEPAD_A,XINPUT_GAMEPAD_B,XINPUT_GAMEPAD_X,XINPUT_GAMEPAD_Y,
        XINPUT_GAMEPAD_LEFT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER,
    };

    //ボタンのUIの設定
    std::array<ButtonUISettings, kMaxButtons> buttonUISettings_{};

    //スキルのUIの設定
    std::array<SkillUISettings, kMaxSkillCount> skillUISettings_{};

    //ボタンのUIの構成
    const std::vector<ButtonConfig> buttonConfigs = {
        { "xbox_button_a_outline.png", "Jump.png", {1000.0f, 630.0f}, {1060.0f, 644.0f}, {1.0f, 1.0f}, {0.3f, 0.3f} },
        { "xbox_button_b_outline.png", "Dash.png", {1048.0f, 582.0f}, {1108.0f, 596.0f}, {1.0f, 1.0f}, {0.3f, 0.3f} },
        { "xbox_button_x_outline.png", "Attack.png", {952.0f, 582.0f}, {880.0f, 596.0f}, {1.0f, 1.0f}, {0.3f, 0.3f} },
        { "xbox_button_y_outline.png", "Fire.png", {1000.0f, 534.0f}, {904.0f, 544.0f}, {1.0f, 1.0f}, {0.3f, 0.3f} },
        { "xbox_lb_outline.png", "Lockon.png", {1070.0f, 429.0f}, {1139.0f, 439.0f}, {1.0f, 1.0f}, {0.3f, 0.3f} },
        { "xbox_rb_outline.png", "Dodge.png", {1070.0f, 484.0f}, {1139.0f, 496.0f}, {1.0f, 1.0f}, {0.3f, 0.3f} },
        { "xbox_rt_outline.png", "Change.png", {1070.0f, 370.0f}, {1139.0f, 382.0f}, {0.5f, 0.5f}, {0.3f, 0.3f} }
    };

    //スキルのUIの構成
    const std::vector<SkillConfig> skillConfigs = {
        { "xbox_button_x_outline.png", "LaunchAttack.png", { 952.0f, 582.0f }, { 790.0f,596.0f }, {1.0f, 1.0f}, {0.3f, 0.3f}, { 955.0f, 580.0f }, { 28.0f,4.0f }},
        { "xbox_button_y_outline.png", "SpinAttack.png", { 1000.0f,534.0f }, { 880.0f,544.0f }, {1.0f, 1.0f}, {0.3f, 0.3f} ,{ 1004.0f,530.0f }, { 28.0f,4.0f }},
    };

    //魔法攻撃用ワーク
    MagicAttackWork magicAttackWork_{};

    //ダメージエフェクトの構造体
    DamageEffect damageEffect_{};

    //ストンプ可能な距離
    const float kStompRange = 6.0f;

    //オーディオハンドル
    uint32_t damageAudioHandle_ = 0;

    //カメラ
    const Camera* camera_ = nullptr;

	//ロックオン
	const Lockon* lockon_ = nullptr;

    //各種パラメーター
    RootParameters rootParameters_{};
    JumpParameters jumpParameters_{};
    DashParameters dashParameters_{};
    AttackParameters groundAttackParameters_{ .comboNum = 4, .attackDistance = 3.0f };
    AttackParameters aerialAttackAttackParameters_{ .comboNum = 3, .attackDistance = 3.0f };
    LaunchAttackParameters launchAttackParameters_{};
    SpinAttackParameters spinAttackParameters_{};
    MagicAttackParameters magicAttackParameters_{};
};

