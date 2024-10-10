#include "PillarStateDespawning.h"
#include "Application/Src/Object/Pillar/Pillar.h"
#include "Application/Src/Object/Pillar/States/PillarStateDespawning.h"

void PillarStateDespawning::Update()
{
	//タイマーを進める
	despawnTimer_ += GameTimer::GetDeltaTime();

	//徐々に透明にする
	alpha_ = std::clamp(1.0f - (despawnTimer_ / despawnDuration_), 0.0f, 1.0f);

	//モデルを取得
	Model* model = pillar_->GetModelComponent()->GetModel();

	//現在の色を取得
	Vector4 currentColor = model->GetMaterial(1)->GetColor();

	//次の色を設定
	Vector4 nextColor = { currentColor.x, currentColor.y, currentColor.z, alpha_ };

	//全てのマテリアルに色を設定
	for (int32_t i = 0; i < model->GetNumMaterials(); ++i)
	{
		model->GetMaterial(i)->SetColor(nextColor);
	}

	//タイマーが一定値を超えていたら破壊する
	if (despawnTimer_ > despawnDuration_)
	{
		pillar_->SetIsDestroy(true);
	}
}