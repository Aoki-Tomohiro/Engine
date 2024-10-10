#pragma once
#include "Application/Src/Object/Character/States/ICharacterState.h"

class Enemy;

class IEnemyState : public ICharacterState
{
public:
	/// <summary>
	/// 敵を設定
	/// </summary>
	/// <param name="enemy">敵</param>
	void SetEnemy(Enemy* enemy) { enemy_ = enemy; };

private:
	Enemy* enemy_ = nullptr;
};