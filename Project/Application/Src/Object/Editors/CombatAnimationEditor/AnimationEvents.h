#pragma once
#include "Engine/Math/Vector3.h"
#include <string>

#pragma region アニメーションイベント

//イベントの種類
enum class EventType
{
    kMovement,       //移動
    kAttack,         //攻撃
    kCancel,         //キャンセル
    kBufferedAction, //先行入力
    kMaxEvent,       //イベントの最大数
};

//アニメーションに関連するイベントの基底構造体
struct AnimationEvent
{
    virtual ~AnimationEvent() = default;
    AnimationEvent(const EventType type) : eventType(type) {};
    EventType eventType{};
    float startEventTime{};
    float endEventTime{};
};

#pragma endregion

#pragma region 移動イベント

//移動の種類
enum class MovementType
{
    kVelocity, // 速度移動
    kEasing,   // イージング移動
};

//移動イベント基底
struct MovementEvent : public AnimationEvent
{
    virtual ~MovementEvent() override = default;
    MovementEvent(const MovementType type) : AnimationEvent(EventType::kMovement), movementType(type) {};
    MovementType movementType = MovementType::kVelocity; //移動の種類
    bool useStickInput = false;                          //スティック入力を使用するかどうか
    bool moveTowardsEnemy = false;                       //敵の方向に移動するかどうか
    bool isProximityStopEnabled = false;                 //相手と接近した際に移動を止めるかどうか
};

//速度を加算する移動イベント
struct VelocityMovementEvent : public MovementEvent
{
    ~VelocityMovementEvent() override = default;
    VelocityMovementEvent() : MovementEvent(MovementType::kVelocity) {};
    float moveSpeed{}; //移動速度
};

//イージングを使用した移動イベント
struct EasingMovementEvent : public MovementEvent
{
    ~EasingMovementEvent() override = default;
    EasingMovementEvent() : MovementEvent(MovementType::kEasing) {};
    Vector3 targetPosition{}; //目標座標
};

#pragma endregion

#pragma region 攻撃イベント

//リアクションの種類
enum class ReactionType
{
    kFlinch,    //のけぞり
    kKnockback, //吹き飛ばし
};

//ヒットSEの種類
enum class HitSEType
{
    kNormal, //通常
};

//攻撃に関連する設定
struct AttackParameters
{
    int32_t maxHitCount{}; //ヒット数
    float hitInterval{};   //ヒット間隔
    float damage{};        //ダメージ量
};

//当たり判定に関連する設定
struct HitboxParameters
{
    Vector3 center{ 0.0f, 0.0f, 0.0f }; //当たり判定の中心
    Vector3 size{ 2.0f, 2.0f, 2.0f };   //当たり判定のサイズ
};

//ノックバックに関連する設定
struct KnockbackParameters
{
    Vector3 velocity{};      //吹き飛ばしの初速度
    Vector3 acceleration{};  //吹き飛ばしの加速度
};

//エフェクトに関連する設定
struct HitEffectConfig
{
    float hitStopDuration = 0.0f;     //ヒット時の一時停止時間
    float cameraShakeDuration = 0.0f; //カメラ揺れの継続時間
    Vector3 cameraShakeIntensity{};   //カメラ揺れの強さ
    std::string hitParticleName{};    //ヒットエフェクトのパーティクル名
    HitSEType hitSEType{};            //ヒット時のSEタイプ
    ReactionType reactionType{};      //リアクションタイプ
};

//攻撃イベント
struct AttackEvent : public AnimationEvent
{
    virtual ~AttackEvent() = default;
    AttackEvent() : AnimationEvent(EventType::kAttack) {};
    AttackParameters attackParameters{};       //攻撃に関する設定
    HitboxParameters hitboxParameters{};       //当たり判定の設定
    KnockbackParameters knockbackParameters{}; //ノックバック設定
    HitEffectConfig effectConfigs{};           //ヒットエフェクトの設定
};

#pragma endregion

#pragma region キャンセルイベント

//キャンセルイベント
struct CancelEvent : public AnimationEvent
{
    virtual ~CancelEvent() = default;
    CancelEvent() : AnimationEvent(EventType::kCancel) {};
    std::string cancelType{};           //キャンセルタイプ
    bool isAnimationCorrectionActive{}; //アニメーションの座標補正をするかどうか
};

#pragma endregion

#pragma region 先行入力イベント

//先行入力イベント
struct BufferedActionEvent : public AnimationEvent
{
    virtual ~BufferedActionEvent() = default;
    BufferedActionEvent() : AnimationEvent(EventType::kBufferedAction) {};
    std::string bufferedActionType{};   //先行入力のタイプ
    bool isAnimationCorrectionActive{}; //アニメーションの座標補正をするかどうか
};

#pragma endregion