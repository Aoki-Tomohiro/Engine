#pragma once
#include "AnimationEvent.h"
#include "Engine/Math/Vector3.h"

//移動の種類
enum class MovementType
{
    kVelocity, // 速度を加算して移動
    kEasing,   // イージングを使用した移動
};

//移動イベント基底
struct MovementEvent : public AnimationEvent
{
    virtual ~MovementEvent() override = default;
    MovementType movementType; // 移動の種類
};

//速度を加算する移動イベント
struct VelocityMovementEvent : public MovementEvent
{
    ~VelocityMovementEvent() override = default;
    Vector3 velocity{}; // 速度
};

//イージングを使用した移動イベント
struct EasingMovementEvent : public MovementEvent
{
    ~EasingMovementEvent() override = default;
    Vector3 targetPosition{}; // 目標座標
};