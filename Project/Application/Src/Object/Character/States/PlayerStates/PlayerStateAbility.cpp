#include "PlayerStateAbility.h"
#include "Application/Src/Object/Character/Player/Player.h"

void PlayerStateAbility::Initialize()
{

}

void PlayerStateAbility::Update()
{
	//アニメーションイベントを実行
	UpdateAnimationState();

	//アニメーションが終了していた場合コンボインデックスをリセットして状態遷移する
	if (character_->GetAnimator()->GetIsAnimationFinished())
	{
		HandleStateTransition();
	}
}

void PlayerStateAbility::OnCollision(GameObject* other)
{
	//衝突処理
	character_->ProcessCollisionImpact(other, false);
}