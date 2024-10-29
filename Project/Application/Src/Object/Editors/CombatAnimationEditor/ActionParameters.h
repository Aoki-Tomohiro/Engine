#pragma once

//パラメーターの種類
enum class ParameterType
{
	kMovement, //移動
	kAttack,   //攻撃
};

//パラメーターの基底構造体
struct ActionParameters
{
	virtual ~ActionParameters() = default;
	ParameterType parameterType; //パラメーターのタイプ
};