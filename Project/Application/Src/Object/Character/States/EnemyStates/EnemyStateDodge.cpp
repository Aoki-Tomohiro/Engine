#include "EnemyStateDodge.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"

void EnemyStateDodge::Initialize()
{
	//アニメーションを再生
	SetAnimationControllerAndPlayAnimation("Dodge");

	//コライダーを無効化する
	character_->GetCollider()->SetCollisionEnabled(false);
}

void EnemyStateDodge::Update()
{
	//アニメーションイベントを実行
	UpdateAnimationState();

	//アニメーションが終了していた場合
	if (character_->GetAnimator()->GetIsAnimationFinished())
	{
		//コライダーを有効化する
		character_->GetCollider()->SetCollisionEnabled(true);
		//状態遷移を実行
		HandleStateTransition();
	}
}