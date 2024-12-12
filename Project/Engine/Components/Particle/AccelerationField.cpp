/**
 * @file AccelerationField.cpp
 * @brief 加速フィールドを管理するクラス
 * @author 青木智滉
 * @date
 */

#include "AccelerationField.h"
#include "Engine/Utilities/GameTimer.h"

void AccelerationField::Initialize(const std::string& name, const float lifeTime)
{
	//名前の初期化
	name_ = name;

	//加速フィールドの寿命の初期化
	deathTimer_ = lifeTime;
}

void AccelerationField::Update()
{
	//加速フィールドの寿命を減らす
	deathTimer_ -= GameTimer::GetDeltaTime();

	//寿命が尽きたら死亡フラグを立てる
	if (deathTimer_ < 0)
	{
		isDead_ = true;
	}
}