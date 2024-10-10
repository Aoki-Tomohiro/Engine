#pragma once

class Enemy;
class GameObject;

class IEnemyState
{
public:
	virtual void Initialize() = 0;

	virtual void Update() = 0;

	virtual void OnCollision(GameObject* other) = 0;

	void SetEnemy(Enemy* enemy) { enemy_ = enemy; };

protected:
	Enemy* enemy_ = nullptr;
};