#pragma once
#include <string>

//イベントの種類
enum class EventType
{
    kMovement, // 移動
    kAttack,   // 攻撃
    kCancel,   // キャンセル
};

//アニメーションに関連するイベントの基底構造体
struct AnimationEvent
{
    virtual ~AnimationEvent() = default;
    std::string eventName{};
    EventType eventType{};
    float startEventTime{};
    float endEventTime{};
};