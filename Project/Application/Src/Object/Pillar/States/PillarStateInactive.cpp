#include "PillarStateInactive.h"
#include "Application/Src/Object/Pillar/Pillar.h"
#include "Application/Src/Object/Pillar/States/PillarStateSpawning.h"

void PillarStateInactive::Update()
{
	//タイマーを進める
	inactiveTimer_ += GameTimer::GetDeltaTime();

	//タイマーが一定値を超えていた場合
	if (inactiveTimer_ > pillar_->GetInactiveDuration())
	{
		//コライダーを有効にする
		pillar_->GetCollider()->SetCollisionEnabled(true);

		//出現状態にする
		pillar_->ChangeState(new PillarStateSpawning());
	}
}