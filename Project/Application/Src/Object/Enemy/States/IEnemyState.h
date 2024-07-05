#pragma once
#include "Engine/3D/Camera/Camera.h"

class Enemy;

class IEnemyState
{
public:
	virtual ~IEnemyState() = default;

	virtual void Initialize() = 0;

	virtual void Update() = 0;

	virtual void Draw(const Camera& camera) = 0;

	const bool GetIsAttack() const { return isAttack_; };

	void SetEnemy(Enemy* enemy) { enemy_ = enemy; };

protected:
	Enemy* enemy_ = nullptr;

	bool isAttack_ = false;
};

