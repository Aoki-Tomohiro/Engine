/**
 * @file GravityField.cpp
 * @brief 重力フィールドを管理するクラス
 * @author 青木智滉
 * @date
 */

#include "GravityField.h"
#include "Engine/Utilities/GameTimer.h"

void GravityField::Initialize(const std::string& name, const float lifeTime)
{
	//名前の初期化
	name_ = name;

	//重力フィールドの寿命の初期化
	deathTimer_ = lifeTime;
}

void GravityField::Update()
{
	//重力フィールドの寿命を減らす
	deathTimer_ -= GameTimer::GetDeltaTime();

	//寿命が尽きたら死亡フラグを立てる
	if (deathTimer_ < 0)
	{
		isDead_ = true;
	}
}