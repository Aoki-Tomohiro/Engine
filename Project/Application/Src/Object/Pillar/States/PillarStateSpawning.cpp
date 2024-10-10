#include "PillarStateSpawning.h"
#include "Application/Src/Object/Pillar/Pillar.h"
#include "Application/Src/Object/Pillar/States/PillarStateDespawning.h"

void PillarStateSpawning::Update()
{
	//タイマーを進める
	spawnTimer_ += GameTimer::GetDeltaTime();

	//イージング係数を計算
	float easingParameter = std::min<float>(1.0f, spawnTimer_ / spawnDuration_);

	//イージングを使用して位置を更新する
	pillar_->UpdatePositionWithEasing(easingParameter);

	//タイマーが一定値を超えていた場合
	if (spawnTimer_ > spawnDuration_)
	{
		//コライダーを有効にする
		pillar_->GetCollider()->SetCollisionEnabled(false);

		//出現終了状態にする
		pillar_->ChangeState(new PillarStateDespawning());
	}
}