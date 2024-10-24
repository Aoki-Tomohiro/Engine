#pragma once

//イベントの種類
enum class EventType
{
    kMove,   // 移動
    kAttack, // 攻撃
};

//アニメーションに関連するイベントの基底構造体
struct AnimationEvent
{
    EventType eventType;
    float startAnimationTime;
    float endAnimationTime;
};