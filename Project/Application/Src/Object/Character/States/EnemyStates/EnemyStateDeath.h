#pragma once
#include "IEnemyState.h"

class EnemyStateDeath : public IEnemyState
{
public:
	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;
};

