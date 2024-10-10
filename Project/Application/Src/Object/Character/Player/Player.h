#pragma once
#include "Engine/3D/Model/AnimationManager.h"
#include "Application/Src/Object/Character/ICharacter.h"
#include "Application/Src/Object/Character/States/PlayerStates/IPlayerState.h"
#include "Application/Src/Object/Lockon/Lockon.h"

class Player : public ICharacter
{
public:
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
    };

    //通常状態のパラメーター
    struct RootParameters
    {
        float walkThreshold = 0.3f; // 歩きの閾値
        float walkSpeed = 9.0f;     // 歩きの移動速度
        float runThreshold = 0.6f;  // 走りの閾値
        float runSpeed = 18.0f;     // 走りの移動速度
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
    /// 状態遷移処理
    /// </summary>
    /// <param name="newState">新しい状態</param>
    void ChangeState(IPlayerState* newState);

    /// <summary>
    /// 敵の方向にプレイヤーを向かせる処理
    /// </summary>
    void LookAtEnemy();

    //アクションフラグの取得・設定
    const bool GetActionFlag(const ActionFlag& actionFlag) const { auto it = actionFlags_.find(actionFlag); return it != actionFlags_.end() && it->second; };
    void SetActionFlag(const ActionFlag& actionFlag, bool value) { actionFlags_[actionFlag] = value; };

    //カメラを設定・取得
    void SetCamera(const Camera* camera) { camera_ = camera; };
    const Camera* GetCamera() const { return camera_; };

	//ロックオンを設定・取得
	void SetLockon(const Lockon* lockon) { lockon_ = lockon; };
    const Lockon* GetLockon() const { return lockon_; };

    //各パラメーターの取得
    const RootParameters& GetRootParameters() const { return rootParameters_; };

private:
    /// <summary>
    /// アニメーターの初期化
    /// </summary>
    void InitializeAnimator() override;

    /// <summary>
    /// UIのスプライトの初期化
    /// </summary>
    void InitializeUISprites() override;

private:
    //状態
    std::unique_ptr<IPlayerState> state_ = nullptr;

    //アクションフラグ
    std::unordered_map<ActionFlag, bool> actionFlags_{};

    //カメラ
    const Camera* camera_ = nullptr;

	//ロックオン
	const Lockon* lockon_ = nullptr;

    //各種パラメーター
    RootParameters rootParameters_{};
};

