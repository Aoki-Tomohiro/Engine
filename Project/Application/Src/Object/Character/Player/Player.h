#pragma once
#include "Engine/3D/Model/AnimationManager.h"
#include "Engine/Components/Input/Input.h"
#include "Application/Src/Object/Character/BaseCharacter.h"
#include "Application/Src/Object/Character/Player/SkillCooldownManager.h"
#include "Application/Src/Object/Character/States/PlayerStates/IPlayerState.h"
#include "Application/Src/Object/Lockon/Lockon.h"
#include <numbers>

/// <summary>
/// プレイヤークラス
/// </summary>
class Player : public BaseCharacter
{
public:
    //スキルの最大数
    static const int32_t kMaxSkillCount = 2;

    //アクションの最大数
    static const int32_t kMaxActionCount = 5;

    //ボタンの種類を示す列挙体
    enum ButtonType
    {
        A, B, X, Y, LB, RB, LT, kMaxButtons,
    };

    //ボタンの入力状態
    struct ButtonState
    {
        bool isPressed = false;   //押されているかどうか
        bool isTriggered = false; //押された瞬間かどうか
        bool isReleased = false;  //離された瞬間かどうか
        int32_t pressedFrame = 0; //押されているフレーム
    };

    //スプライトの基本設定
    struct SpriteSettings
    {
        std::unique_ptr<Sprite> sprite = nullptr;  //スプライト
        Vector2 position{ 0.0f, 0.0f };            //スプライトの位置
        Vector2 scale{ 1.0f, 1.0f };               //スプライトのスケール
    };

    //ボタンのUI設定
    struct ButtonUISettings
    {
        SpriteSettings buttonSprite{}; //ボタンのスプライト設定
        SpriteSettings fontSprite{};   //フォントのスプライト設定
    };

    //スキルのUI設定
    struct SkillUISettings
    {
        ButtonUISettings buttonSettings{};           //スキルボタンの設定
        std::unique_ptr<Sprite> cooldownBarSprite{}; //スキルクールダウンバーのスプライト
    };

    //ボタンのUI設定情報
    struct ButtonConfig
    {
        ButtonType buttonType;                //対応するボタン
        std::string buttonTexture;            //ボタンのテクスチャファイル名
        std::string fontTexture;              //フォントのテクスチャファイル名
        Vector2 buttonPosition{ 0.0f, 0.0f }; //ボタンの位置
        Vector2 fontPosition{ 0.0f, 0.0f };   //フォントの位置
        Vector2 buttonScale{ 1.0f, 1.0f };    //ボタンのスケール
        Vector2 fontScale{ 1.0f, 1.0f };      //フォントのスケール
    };

    //スキルのUI設定情報
    struct SkillConfig
    {
        ButtonConfig buttonConfig{};            //スキルボタンの設定
        Vector2 skillBarPosition{ 0.0f, 0.0f }; //スキルバーの位置
        Vector2 skillBarScale{ 1.0f, 1.0f };    //スキルバーのスケール
    };

    //アクションフラグ
    enum class ActionFlag
    {
        kCanStomp,                 //ストンプが可能かどうか
        kDashAttackEnabled,        //ダッシュ攻撃が有効かどうか
        kMagicAttackEnabled,       //魔法攻撃が有効かどうか
        kChargeMagicAttackEnabled, //溜め魔法攻撃が有効かどうか
        kFallingAttackEnabled,     //落下攻撃が有効かどうか
        kAbility1Enabled,          //アビリティ1が有効かどうか
        kAbility2Enabled,          //アビリティ2が有効かどうか
        kIsAttacking,              //攻撃したかどうか
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
        float walkThreshold = 0.3f; //歩きの閾値
        float walkSpeed = 9.0f;     //歩きの移動速度
        float runThreshold = 0.6f;  //走りの閾値
        float runSpeed = 18.0f;     //走りの移動速度
    };

    //魔法攻撃用のパラメーター
    struct MagicAttackParameters
    {
        float chargeTimeThreshold = 0.8f;   //溜め魔法のため時間
        float cooldownTime = 0.6f;          //通常魔法のクールタイム
        float magicProjectileSpeed = 96.0f; //魔法の速度
    };

    //スキルのパラメーター
    struct SkillParameters
    {
        std::string name{};              //名前
        float cooldownDuration = 0.0f;   //クールダウンの時間
        bool canUseOnGroundOnly = false; //地面にいる時しか発動できないかどうか
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
    /// ダメージとノックバックを適用
    /// </summary>
    /// <param name="knockbackSettings">ノックバックの設定</param>
    /// <param name="damage">ダメージ</param>
    /// <param name="transitionToStun">スタン状態に遷移するかどうか</param>
    void ApplyDamageAndKnockback(const KnockbackParameters& knockbackSettings, const float damage, const bool transitionToStun) override;

    /// <summary>
    /// 敵の方向にプレイヤーを向かせる処理
    /// </summary>
    void LookAtEnemy();

    /// <summary>
    /// 魔法攻撃のクールダウンをリセット
    /// </summary>
    void ResetMagicAttackCooldownTime() { magicAttackWork_.cooldownTimer = magicAttackParameters_.cooldownTime; };

    /// <summary>
    /// アビリティの名前を取得
    /// </summary>
    /// <param name="isSecond">`true`で2番目の名前、`false`で最初の名前。</param>
    /// <returns>アビリティの名前</returns>
    const std::string& GetAbilityName(const bool isSecond) const { return isSecond ? skillPairSets_[activeSkillSetIndex_].second.name : skillPairSets_[activeSkillSetIndex_].first.name; };

    //アクションフラグの取得・設定
    const bool GetActionFlag(const ActionFlag& actionFlag) const { auto it = actionFlags_.find(actionFlag); return it != actionFlags_.end() && it->second; };
    void SetActionFlag(const ActionFlag& actionFlag, bool value) { actionFlags_[actionFlag] = value; };

    //スキルのクールダウン確認・リセット
    const bool GetIsCooldownComplete(const std::string& name) const { return skillCooldownManager_->IsCooldownComplete(name); };
    void ResetCooldown(const std::string& name) { skillCooldownManager_->ResetCooldown(name); };

    //ロックオンを設定・取得
    void SetLockon(const Lockon* lockon) { lockon_ = lockon; };
    const Lockon* GetLockon() const { return lockon_; };

    //各パラメーターの取得
    const RootParameters& GetRootParameters() const { return rootParameters_; };
    const MagicAttackParameters& GetMagicAttackParameters() const { return magicAttackParameters_; };

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
    /// スキルクールダウンマネージャーの初期化
    /// </summary>
    void InitializeSkillCooldownManager();

    /// <summary>
    /// ダメージエフェクトのスプライトを生成
    /// </summary>
    void InitializeDamageEffectSprite();

    /// <summary>
    /// ボタンのUIスプライトを設定
    /// </summary>
    void SetButtonUISprite(ButtonUISettings& uiSettings, const ButtonConfig& config);

    /// <summary>
    /// スキルのUIスプライトを設定
    /// </summary>
    void SetSkillUISprite(SkillUISettings& uiSettings, const SkillConfig& config);

    /// <summary>
    /// 全てのボタンの入力状態の更新
    /// </summary>
    void UpdateButtonStates();

    /// <summary>
    /// ボタン入力状態の更新
    /// </summary>
    /// <param name="buttonIndex">ボタンのインデックス</param>
    void UpdateButtonState(const int32_t buttonIndex);

    /// <summary>
    /// ボタンが押されているかを確認
    /// </summary>
    /// <param name="buttonIndex">ボタンのインデックス</param>
    /// <returns>ボタンが押されているか</returns>
    bool CheckButtonPress(const int32_t buttonIndex) const;
         
    /// <summary>
    /// ボタンが離されているかを確認
    /// </summary>
    /// <param name="buttonIndex">ボタンのインデックス</param>
    /// <returns>ボタンが離されているか</returns>
    bool CheckButtonRelease(const int32_t buttonIndex) const;

    /// <summary>
    /// クールダウンタイマーの更新
    /// </summary>
    void UpdateCooldownTimer();

    /// <summary>
    /// ストンプのフラグの更新
    /// </summary>
    void UpdateStompFlag();

    /// <summary>
    /// 魔法攻撃の更新
    /// </summary>
    void UpdateMagicAttack();

    /// <summary>
    /// 魔法を溜める処理
    /// </summary>
    void HandleMagicCharge();

    /// <summary>
    /// チャージ継続処理
    /// </summary>
    void ContinueCharging();

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
    /// UIの更新
    /// </summary>
    void UpdateUI();

    /// <summary>
    /// ボタンのスケールの更新
    /// </summary>
    /// <param name="buttonState">ボタン入力の状態</param>
    /// <param name="baseScale">基本のスケール</param>
    /// <param name="spriteSetting">スプライトの設定</param>
    /// <param name="buttonType">ボタンの種類</param>
    void UpdateButtonScale(const ButtonState& buttonState, const Vector2& baseScale, SpriteSettings& spriteSetting, const ButtonType buttonType);

    /// <summary>
    /// UIの編集
    /// </summary>
    void EditUI();

    /// <summary>
    /// ボタンの設定を編集
    /// </summary>
    /// <param name="config">ボタンの設定</param>
    /// <param name="uiSettings">UIの設定</param>
    void EditButtonConfig(ButtonConfig& config, ButtonUISettings& uiSettings);

    /// <summary>
    /// スキルの設定を編集
    /// </summary>
    /// <param name="config">スキルの設定</param>
    /// <param name="uiSettings">スキルのUI設定</param>
    /// <param name="index">スキルのインデックス</param>
    void EditSkillConfig(SkillConfig& config, SkillUISettings& uiSettings, const int32_t index);

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
    /// 落下攻撃の条件を確認して発動
    /// </summary>
    /// <returns>発動したかどうか</returns>
    bool CheckFallingAttack();

    /// <summary>
    /// アビリティの条件を確認して発動
    /// </summary>
    /// <returns>発動したかどうか</returns>
    bool CheckAndTriggerAbility();

    /// <summary>
    /// アビリティが使用可能かを確認
    /// </summary>
    /// <param name="skill">スキル</param>
    /// <param name="button">ボタン</param>
    /// <returns>アビリティを使用できるかどうか</returns>
    bool IsAbilityAvailable(const SkillParameters& skill, const Player::ButtonType button);

private:
    //インプット
    Input* input_ = nullptr;

    //スキルクールダウンマネージャー
    std::unique_ptr<SkillCooldownManager> skillCooldownManager_ = nullptr;

    //スキルのパラメーター
    const std::array<std::pair<SkillParameters, SkillParameters>, kMaxSkillCount / 2> skillPairSets_ = { {
        {SkillParameters{"LaunchAttack", 2.0f, true}, SkillParameters{"SpinAttack", 1.0f, false}}}
    };

    //現在選択中のスキルセット
    int32_t activeSkillSetIndex_ = 0;

    //アクションフラグ
    std::unordered_map<ActionFlag, bool> actionFlags_{};

    //ボタンの状態
    std::array<ButtonState, kMaxButtons> buttonStates_{};

    //ボタン定数を保持する配列
    const std::array<WORD, kMaxButtons> buttonMappings_{
        XINPUT_GAMEPAD_A,XINPUT_GAMEPAD_B,XINPUT_GAMEPAD_X,XINPUT_GAMEPAD_Y,
        XINPUT_GAMEPAD_LEFT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER,
    };

    //ボタンのUIの設定
    std::array<ButtonUISettings, kMaxActionCount> buttonUISettings_{};

    //ボタンのUIの構成
    std::array<ButtonConfig, kMaxActionCount> buttonConfigs_{ {
        { ButtonType::A, "xbox_button_a_outline.png", "Jump.png", {1032.0f, 662.0f}, {1060.0f, 644.0f}, {1.0f, 1.0f}, {0.3f, 0.3f} },
        { ButtonType::X, "xbox_button_x_outline.png", "Attack.png", {974.0f, 604.0f}, {870.0f, 586.0f}, {1.0f, 1.0f}, {0.3f, 0.3f} },
        { ButtonType::LB, "xbox_lb_outline.png", "Dash.png", {1142.0f, 451.0f}, {1172.0f, 429.0f}, {1.0f, 1.0f}, {0.3f, 0.3f} },
        { ButtonType::RB, "xbox_rb_outline.png", "Dodge.png", {1142.0f, 506.0f}, {1182.0f, 486.0f}, {1.0f, 1.0f}, {0.3f, 0.3f} },
        { ButtonType::LT, "xbox_rt_outline.png", "Fire.png", {1142.0f, 390.0f}, {1172.0f, 372.0f}, {0.5f, 0.5f}, {0.3f, 0.3f} }}
    };

    //スキルのUIの設定
    std::array<SkillUISettings, kMaxSkillCount> skillUISettings_{};

    //スキルのUIの構成
    std::array<SkillConfig, kMaxSkillCount> skillConfigs_ = { {
        { ButtonType::Y, "xbox_button_y_outline.png", "LaunchAttack.png", {1032.0f, 546.0f}, {840.0f, 524.0f}, {1.0f, 1.0f}, {0.3f, 0.3f}, { 1004.0f, 500.0f }, { 28.0f,4.0f }},
        { ButtonType::B, "xbox_button_b_outline.png", "SpinAttack.png", {1090.0f, 604.0f}, {1118.0f, 586.0f}, {1.0f, 1.0f}, {0.3f, 0.3f} ,{ 1062.0f, 558.0f }, { 28.0f,4.0f }},}
    };

    //魔法攻撃用ワーク
    MagicAttackWork magicAttackWork_{};

    //ダメージエフェクトの構造体
    DamageEffect damageEffect_{};

    //ロックオン
    const Lockon* lockon_ = nullptr;

    //ストンプ可能な距離
    const float kStompRange_ = 6.0f;

    //トリガーの入力閾値
    const float kTriggerThreshold = 0.7f;

    //各種パラメーター
    RootParameters rootParameters_{};
    MagicAttackParameters magicAttackParameters_{};
};

