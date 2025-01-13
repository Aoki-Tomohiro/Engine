/**
 * @file Player.h
 * @brief プレイヤーを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Components/Input/Input.h"
#include "Application/Src/Object/Character/BaseCharacter.h"
#include "Application/Src/Object/Character/Player/SkillCooldownManager.h"
#include "Application/Src/Object/Character/States/PlayerStates/AbstractPlayerState.h"
#include "Application/Src/Object/Lockon/Lockon.h"
#include <numbers>

class Player : public BaseCharacter
{
public:
    //スキルの最大数
    static const int32_t kMaxSkillCount = 2;

    //アクションの最大数
    static const int32_t kMaxActionCount = 6;

    //ボタンの種類を示す列挙体
    enum ButtonType
    {
        A, B, X, Y, LB, RB, LT, XA, kMaxButtons,
    };

    //アクションフラグ
    enum class ActionFlag
    {
        kCanStomp,                 //ストンプが可能かどうか
        kDashAttackEnabled,        //ダッシュ攻撃が有効かどうか
        kMagicAttackEnabled,       //魔法攻撃が有効かどうか
        kChargeMagicAttackEnabled, //溜め魔法攻撃が有効かどうか
        kFallingAttackEnabled,     //落下攻撃が有効かどうか
        kIsAttacking,              //攻撃したかどうか
        kJustDodge,                //ジャスト回避したかどうか
        kCounterAttack,            //カウンター攻撃したかどうか
    };

    //ボタンの入力状態
    struct ButtonState
    {
        bool isPressed = false;   //押されているかどうか
        bool isTriggered = false; //押された瞬間かどうか
        bool isReleased = false;  //離された瞬間かどうか
        int32_t pressedFrame = 0; //押されているフレーム
    };

    //UIとその元の座標を管理する構造体
    struct UIElementWithPosition
    {
        UIElement* ui = nullptr; //UI要素
        Vector2 basePosition{};  //元の座標
    };

    //ダイナミックUIとその元の座標を管理する構造体
    struct DynamicUIWithPosition 
    {
        DynamicUI* ui = nullptr; //ダイナミックUI要素
        Vector2 basePosition{};  //元の座標
    };

    //QTEのUI
    struct QTEUI 
    {
        UIElementWithPosition buttonUI;   //ボタンUIとその基準位置
        UIElementWithPosition actionUI;   //アクション名UIとその基準位置
        DynamicUIWithPosition barUI;      //受付時間バーUIとその基準位置
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
        float walkSpeed = 6.0f;     //歩きの移動速度
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
        int skillSetIndex = -1;          //スキルセットの番号
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
    const MagicAttackParameters& GetMagicAttackParameters() const { return magicAttackParameters_; }

private:
    /// <summary>
    /// アクションマップの初期化
    /// </summary>
    void InitializeActionMap() override;

    /// <summary>
    /// UIのスプライトの初期化
    /// </summary>
    void InitializeUISprites() override;

    /// <summary>
    /// スキルのUIを設定
    /// </summary>
    void SetupSkillUIForPairs();

    /// <summary>
    /// スキルUIの設定処理を行う関数
    /// </summary>
    /// <param name="index">インデックス</param>
    /// <param name="skillParameters">スキルのパラメーター</param>
    /// <param name="index">スキルの番号</param>
    void SetupSkillUI(size_t index, const SkillParameters& skillParameters, size_t skillIndex);

    /// <summary>
    /// QTEのUIを設定
    /// </summary>
    void SetupQTEUI();

    /// <summary>
    /// スキルクールダウンマネージャーの初期化
    /// </summary>
    void InitializeSkillCooldownManager();

    /// <summary>
    /// ダメージエフェクトのスプライトを生成
    /// </summary>
    void InitializeDamageEffectSprite();

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
    /// ダメージエフェクトの更新
    /// </summary>
    void UpdateDamageEffect();

    /// <summary>
    /// QTEの要素の更新
    /// </summary>
    void UpdateQTEElements();

    /// <summary>
    /// QTEUIの座標と時間を設定
    /// </summary>
    /// <param name="qteUI">設定するQTEUI</param>
    /// <param name="position">座標</param>
    /// <param name="duration">受付時間</param>
    /// <param name="elapsedTime">経過時間</param>
    void SetQTEUIPositionAndTime(QTEUI& qteUI, const Vector2& position, float duration, float elapsedTime);

    /// <summary>
    /// 落下攻撃の条件を確認して発動
    /// </summary>
    /// <returns>発動したかどうか</returns>
    bool CheckFallingAttack();

    /// <summary>
    /// 指定されたスキルが使用可能な状態かどうかを判別
    /// </summary>
    /// <param name="skill">スキルのパラメーター</param>
    /// <returns>スキルが使用可能かどうか</returns>
    bool IsAbilityUsable(const SkillParameters& skill) const;

private:
    //インプット
    Input* input_ = nullptr;

    //スキルクールダウンマネージャー
    std::unique_ptr<SkillCooldownManager> skillCooldownManager_ = nullptr;

    //スキルのパラメーター
    const std::array<std::pair<SkillParameters, SkillParameters>, kMaxSkillCount / 2> skillPairSets_ = { {
        {SkillParameters{"LaunchAttack", 4.0f, true, 0}, SkillParameters{"SpinAttack", 3.0f, false, 0}}}
    };

    //スキルのUI
    std::array<std::pair<DynamicUI*, DynamicUI*>, kMaxSkillCount / 2> skillUI_{};

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

    //QTEのUI
    std::map<std::string, QTEUI> qteUIMap_{};

    //QTEのUiの距離
    const float qteUiDistance_ = 160.0f;

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

