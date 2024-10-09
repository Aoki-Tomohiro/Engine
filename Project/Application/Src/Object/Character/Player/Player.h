#pragma once
#include "Application/Src/Object/Character/ICharacter.h"
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

    //アクションフラグの取得・設定
    const bool GetActionFlag(const ActionFlag& actionFlag) const { auto it = actionFlags_.find(actionFlag); return it != actionFlags_.end() && it->second; };
    void SetActionFlag(const ActionFlag& actionFlag, bool value) { actionFlags_[actionFlag] = value; };

	//ロックオンを設定
	void SetLockon(const Lockon* lockon) { lockon_ = lockon; };

	//カメラを設定
	void SetCamera(const Camera* camera) { camera_ = camera; };

private:
    //アクションフラグ
    std::unordered_map<ActionFlag, bool> actionFlags_{};

	//ロックオン
	const Lockon* lockon_ = nullptr;

	//カメラ
	const Camera* camera_ = nullptr;
};

