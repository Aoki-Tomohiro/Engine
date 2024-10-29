#pragma once
#include "ActionParameters.h"
#include "Engine/Math/Vector3.h"
#include <string>

//リアクションの種類
enum class ReactionType
{
    kFlinch,    // のけぞり
    kKnockback, // 吹き飛ばし
};

//ヒットSEの種類
enum class HitSEType
{
    kNormal,
};

//攻撃ヒット時に関連するパラメーター
struct HitParameters
{
    int32_t hitCount = 0;      // ヒット数
    float hitInterval = 0.0f;  // ヒット間隔
    float damage = 0.0f;       // ダメージ量
};

//当たり判定に関連するパラメーター
struct HitboxParameters
{
    Vector3 center{ 0.0f, 0.0f, 0.0f }; // 当たり判定の中心
    Vector3 size{ 2.0f, 2.0f, 2.0f };   // 当たり判定のサイズ
};

//ノックバックに関連するパラメーター
struct KnockbackParameters
{
    Vector3 velocity{};      // 吹き飛ばしの初速度
    Vector3 acceleration{};  // 吹き飛ばしの加速度
};

//エフェクトに関連する設定
struct HitEffectConfig
{
    float hitStopDuration = 0.0f;                      // ヒット時の一時停止時間
    float cameraShakeDuration = 0.0f;                  // カメラ揺れの継続時間
    Vector3 cameraShakeIntensity{};                    // カメラ揺れの強さ
    std::string hitParticleName{};                     // ヒットエフェクトのパーティクル名
    HitSEType hitSEType = HitSEType::kNormal;          // ヒット時のSEタイプ
    ReactionType reactionType = ReactionType::kFlinch; // リアクションタイプ
};

//攻撃パラメーター
struct AttackEvent : public ActionParameters
{
    virtual ~AttackEvent() = default;
    HitParameters hitParameters{};             // 攻撃ヒット時に関するパラメーター
    HitboxParameters hitboxParameters{};       // 当たり判定のパラメーター
    KnockbackParameters knockbackParameters{}; // ノックバックパラメーター
    HitEffectConfig effectConfigs{};           // ヒットエフェクトのパラメーター
};