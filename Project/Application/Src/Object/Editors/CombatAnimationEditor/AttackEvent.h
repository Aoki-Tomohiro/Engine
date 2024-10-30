#pragma once
#include "AnimationEvent.h"
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

//攻撃に関連する設定
struct AttackParameters
{
    int32_t maxHitCount{};     // ヒット数
    float hitInterval{};       // ヒット間隔
    float damage{};            // ダメージ量
    int32_t currentHitCount{}; // 現在のヒット数
    float elapsedAttackTime{}; // 攻撃判定が発生するまでの経過時間
};

//当たり判定に関連する設定
struct HitboxParameters
{
    Vector3 center{ 0.0f, 0.0f, 0.0f }; // 当たり判定の中心
    Vector3 size{ 2.0f, 2.0f, 2.0f };   // 当たり判定のサイズ
};

//ノックバックに関連する設定
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

//攻撃イベント
struct AttackEvent : public AnimationEvent
{
    virtual ~AttackEvent() = default;
    AttackParameters attackParameters{};       // 攻撃に関する設定
    HitboxParameters hitboxParameters{};       // 当たり判定の設定
    KnockbackParameters knockbackParameters{}; // ノックバック設定
    HitEffectConfig effectConfigs{};           // ヒットエフェクトの設定
};