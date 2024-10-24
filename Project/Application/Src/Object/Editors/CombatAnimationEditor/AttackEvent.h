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
struct AttackEventSettings
{
    int32_t hitCount = 0;
    float hitInterval = 0.0f;
    float damage = 0.0f;
};

//当たり判定に関連する設定
struct HitboxEventSettings
{
    Vector3 center{ 0.0f, 0.0f, 0.0f };
    Vector3 size{ 2.0f, 2.0f, 2.0f };
};

//ノックバックに関連する設定
struct KnockbackEventSettings
{
    Vector3 velocity{};
    Vector3 acceleration{};
};

//エフェクトに関連する設定
struct HitEffectSettings
{
    float hitStopDuration = 0.0f;
    float cameraShakeDuration = 0.0f;
    Vector3 cameraShakeIntensity{};
    std::string hitParticleName{};
    HitSEType hitSEType = HitSEType::kNormal;
    ReactionType reactionType = ReactionType::kFlinch;
};

//攻撃イベント
struct AttackEvent : public AnimationEvent
{
    AttackEventSettings attackSettings{};
    HitboxEventSettings hitboxSettings{};
    KnockbackEventSettings knockbackSettings{};
    HitEffectSettings effectSettings{};
};