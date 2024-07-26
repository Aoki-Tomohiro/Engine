#pragma once
#include "IEnemyState.h"

class EnemyStateDash : public IEnemyState
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

private:
};

