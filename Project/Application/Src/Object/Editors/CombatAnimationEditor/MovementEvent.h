#pragma once
#include "AnimationEvent.h"
#include "Engine/Math/Vector3.h"

//移動の種類
enum class MovementType
{
    AddVelocity, // 速度を加算して移動
    Easing,      // イージングを使用した移動
};

//移動イベント基底
struct MovementEvent : public AnimationEvent
{
    MovementType movementType; // 移動の種類
};

//速度を加算する移動イベント
struct VelocityMovementEvent : public MovementEvent
{
    Vector3 velocity; // 速度
};

//イージングを使用した移動イベント
struct EasingMovementEvent : public MovementEvent
{
    Vector3 startPosition; // 開始座標
    Vector3 endPosition;   // 終了座標
};