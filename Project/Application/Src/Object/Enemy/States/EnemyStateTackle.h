#pragma once
#include "IEnemyState.h"

class EnemyStateTackle : public IEnemyState
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera);

    const bool GetIsAttack() const { return isAttack_; };

private:
    bool isAttack_ = false;
};

