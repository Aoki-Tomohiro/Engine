#pragma once
#include "Application/Src/Object/Enemy/States/IEnemyState.h"

class EnemyStateStun : public IEnemyState
{
public:
	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;
};
