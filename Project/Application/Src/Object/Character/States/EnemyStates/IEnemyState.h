#pragma once
#include "Application/Src/Object/Character/States/ICharacterState.h"

class Enemy;

class IEnemyState : public ICharacterState
{
public:
	virtual ~IEnemyState() override = default;

	//敵を取得
	Enemy* GetEnemy() const;

protected:

};