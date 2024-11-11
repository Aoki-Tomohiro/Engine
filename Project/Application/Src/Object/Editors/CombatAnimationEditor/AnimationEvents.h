#pragma once
#include "Engine/Math/Vector3.h"
#include <string>

#pragma region イベント基底構造体

// イベントの種類を表す列挙体
enum class EventType
{
    kMovement,        //移動イベント
    kRotation,        //回転イベント
    kAttack,          //攻撃イベント
    kCameraAnimation, //カメラアニメーションイベント
    kCancel,          //キャンセルイベント
    kBufferedAction,  //先行入力イベント
    kMaxEvent,        //イベントの最大数
};

//アニメーションイベントの基底構造体
struct AnimationEvent
{
    virtual ~AnimationEvent() = default;

    //コンストラクタ
    AnimationEvent(const EventType type) : eventType(type) {}

    EventType eventType{};  //イベントの種類
    float startEventTime{}; //イベント開始時間
    float endEventTime{};   //イベント終了時間
};

#pragma endregion

#pragma region 移動イベント

//移動の種類
enum class MovementType
{
    kVelocity, //速度移動
    kEasing,   //イージング移動
};

//イージングの種類
enum class EasingType
{
    kLinear,    //リニア
    kEaseIn,    //イーズイン
    kEaseOut,   //イーズアウト
    kEaseInOut, //イーズインアウト
};

//移動イベントの基底構造体
struct MovementEvent : public AnimationEvent
{
    virtual ~MovementEvent() override = default;

    MovementEvent(const MovementType type) : AnimationEvent(EventType::kMovement), movementType(type) {}

    MovementType movementType = MovementType::kVelocity; //移動の種類
    bool useStickInput = false;                          //スティック入力を使用するか
    bool moveTowardsEnemy = false;                       //敵の方向に移動するか
    bool isProximityStopEnabled = false;                 //近接停止を有効にするか
    bool rotateTowardsMovement = false;                  //移動方向に回転するか
};

//速度移動イベント
struct VelocityMovementEvent : public MovementEvent
{
    VelocityMovementEvent() : MovementEvent(MovementType::kVelocity) {}

    Vector3 velocity{}; //移動速度
};

//イージング移動イベント
struct EasingMovementEvent : public MovementEvent
{
    EasingMovementEvent() : MovementEvent(MovementType::kEasing) {}

    EasingType easingType{};  //イージングの種類
    Vector3 targetPosition{}; //目標位置
};

#pragma endregion

#pragma region 回転イベント

//回転イベントの構造体
struct RotationEvent : public AnimationEvent
{
    RotationEvent() : AnimationEvent(EventType::kRotation) {}

    EasingType easingType{};   //イージングの種類
    Vector3 rotationAxis{};    //回転軸
    float rotationAngle{};     //総回転角度
};

#pragma endregion

#pragma region 攻撃イベント

//攻撃のリアクションタイプ
enum class ReactionType
{
    kFlinch,    //のけぞり
    kKnockback, //吹き飛ばし
};

//ヒット音の種類
enum class HitSEType
{
    kNormal, //通常ヒット音
};

//攻撃に関する設定
struct AttackParameters
{
    int32_t maxHitCount{}; //最大ヒット数
    float hitInterval{};   //ヒット間隔
    float damage{};        //ダメージ量
};

//当たり判定に関する設定
struct HitboxParameters
{
    Vector3 center{ 0.0f, 0.0f, 3.0f }; //中心位置
    Vector3 size{ 2.0f, 2.0f, 2.0f };   //サイズ
};

//ノックバックの設定
struct KnockbackParameters
{
    Vector3 velocity{};      //初速
    Vector3 acceleration{};  //加速度
};

//ヒットエフェクトの設定
struct HitEffectConfig
{
    float hitStopDuration = 0.0f;     //ヒット時の一時停止時間
    float cameraShakeDuration = 0.0f; //カメラ揺れ時間
    Vector3 cameraShakeIntensity{};   //カメラ揺れ強度
    std::string hitParticleName{};    //ヒット時のエフェクト
    HitSEType hitSEType{};            //ヒット時の音
    ReactionType reactionType{};      //リアクションタイプ
};

//攻撃イベント
struct AttackEvent : public AnimationEvent
{
    AttackEvent() : AnimationEvent(EventType::kAttack) {}

    AttackParameters attackParameters{};       //攻撃の設定
    HitboxParameters hitboxParameters{};       //当たり判定
    KnockbackParameters knockbackParameters{}; //ノックバック設定
    HitEffectConfig effectConfigs{};           //ヒットエフェクト設定
};

#pragma endregion

#pragma region カメラアニメーションイベント

//カメラアニメーションのトリガー条件
enum class CameraAnimationTrigger
{
    kActionStart, //アクション開始時
    kImpact,      //攻撃ヒット時
};

//カメラアニメーションイベント
struct CameraAnimationEvent : public AnimationEvent
{
    CameraAnimationEvent() : AnimationEvent(EventType::kCameraAnimation) {}

    std::string animationName{};       //アニメーション名
    float animationSpeed = 1.0f;       //アニメーションの速度
    bool syncWithCharacterAnimation{}; //キャラクターのアニメーションに同期させるかどうか
    CameraAnimationTrigger trigger{};  //アニメーションが再生される条件
};

#pragma endregion

#pragma region キャンセルイベント

//キャンセルイベント
struct CancelEvent : public AnimationEvent
{
    CancelEvent() : AnimationEvent(EventType::kCancel) {}

    std::string cancelType{}; //キャンセルタイプ
};

#pragma endregion

#pragma region 先行入力イベント

//先行入力イベント
struct BufferedActionEvent : public AnimationEvent
{
    BufferedActionEvent() : AnimationEvent(EventType::kBufferedAction) {}

    std::string bufferedActionType{}; //先行入力タイプ
};

#pragma endregion

#pragma region 定数文字列配列

//イベントタイプに対応する文字列配列
namespace
{
    const char* EVENT_TYPES[] = { "Movement", "Rotation", "Attack", "CameraAnimation", "Cancel", "Buffered Action"}; //イベントタイプ
    const char* MOVEMENT_TYPES[] = { "Velocity", "Easing" }; //移動イベントタイプ
    const char* EASING_TYPES[] = { "Linear", "EaseIn", "EaseOut", "EaseInOut" }; //イージングタイプ
    const char* HIT_SE_TYPES[] = { "Normal" }; //ヒット音SEタイプ
    const char* REACTION_TYPES[] = { "Flinch", "Knockback" }; //リアクションタイプ
    const char* CAMERA_ANIMATION_TRIGGERS[] = { "ActionStart", "Impact" }; //カメラアニメーションのトリガー条件
    const char* CANCEL_TYPES[] = { "Move", "Dodge", "Dash", "Attack", "Stomp", "Magic", "ChargeMagic", "FallingAttack", "Ability1" ,"Ability2" }; //キャンセルタイプ
    const char* BUFFERED_ACTION_TYPES[] = { "Move", "Dodge", "Dash", "Attack", "Stomp", "Magic", "ChargeMagic", "FallingAttack", "Ability1" ,"Ability2" }; //先行入力タイプ
}

#pragma endregion