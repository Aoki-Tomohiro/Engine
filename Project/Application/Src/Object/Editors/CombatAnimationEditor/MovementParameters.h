#pragma once
#include "ActionParameters.h"
#include "Engine/Math/Vector3.h"

//移動の種類
enum class MovementType
{
	kVelocity, //速度による移動
	kEasing,   //イージングによる移動
};

//移動パラメーターの基底構造体
struct MovementParameters : public ActionParameters
{
	virtual ~MovementParameters() override = default;
	MovementType movementType; //移動の種類
};

//速度移動のパラメーター
struct VelocityMovementParameters : public MovementParameters
{
	~VelocityMovementParameters() override = default;
	Vector3 velocity{}; //移動の種類
};

//イージングのタイプ
enum class EasingType
{
	kNormal, //通常
};

//イージング移動のパラメーター
struct EasingMovementParameters : public MovementParameters
{
	~EasingMovementParameters() override = default;
	EasingType easingType;
	Vector3 distance{};
};