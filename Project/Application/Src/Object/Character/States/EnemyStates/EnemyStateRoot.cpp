#include "EnemyStateRoot.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"

void EnemyStateRoot::Initialize()
{
	//アニメーションブレンドを有効にする
	character_->GetAnimator()->SetIsBlending(true);

	//アニメーションブレンドの時間を設定
	character_->GetAnimator()->SetBlendDuration(0.2f);

	//アニメーションを再生
	character_->GetAnimator()->PlayAnimation("Idle", 1.0f, true, {0.0f, 0.0f, 1.0f});
}

void EnemyStateRoot::Update()
{

}

void EnemyStateRoot::OnCollision(GameObject* other)
{
	//衝突処理
	character_->ProcessCollisionImpact(other, true);
}