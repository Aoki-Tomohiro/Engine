/**
 * @file CharacterStateFactory.cpp
 * @brief キャラクターのスタン状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#include "CharacterStateStun.h"
#include "Application/Src/Object/Character/BaseCharacter.h"

void CharacterStateStun::Initialize()
{
	//スタン状態のフラグを解除
	character_->SetIsStunTriggered(false);

	//アニメーションブレンドを無効にする
	character_->GetAnimator()->SetIsBlending(false);

	//現在のリアクションタイプを取得
	currentReactionType_ = character_->GetPosition().y == 0.0f ? character_->GetKnockbackParameters().reactionType : ReactionType::kKnockback;

	//武器のトレイルをなくす
	character_->GetWeapon()->SetIsTrailActive(false);
	
	//武器の判定をなくす
	character_->GetWeapon()->SetIsAttack(false);

	//アニメーションの再生とアニメーションコントローラーを取得
	PlayReactionAnimation(currentReactionType_);
}

void CharacterStateStun::Update()
{
	//ノックバックを適用
	if (isStunActive_)
	{
		character_->ApplyKnockback();
	}

	//アニメーションイベントを実行
	UpdateAnimationState();

	//地面に埋まらないように座標を補正
	ClampPositionToGround();

	//アニメーションが終了しているかつ地面にいる場合
	if (character_->GetAnimator()->GetIsAnimationFinished() && character_->GetPosition().y == 0.0f)
	{
		HandleStateTransitionBasedOnReaction();
	}
}

void CharacterStateStun::OnCollision(GameObject* other)
{
	//衝突処理
	character_->ProcessCollisionImpact(other, isStunActive_);
}

void CharacterStateStun::PlayReactionAnimation(const ReactionType reactionType)
{
	//リアクションの種類に応じて再生するアニメーションを変える
	switch (reactionType)
	{
	case ReactionType::kFlinch:
		SetAnimationControllerAndPlayAnimation("HitStun");
		break;
	case ReactionType::kKnockback:
		SetAnimationControllerAndPlayAnimation("Knockdown");
		break;
	}
}

void CharacterStateStun::ClampPositionToGround()
{
	//現在のキャラクターの位置を取得
	Vector3 currentPosition = character_->GetPosition();

	//地面に到達した場合
	if (currentPosition.y < 0.0f)
	{
		//キャラクターの位置を地面に合わせる
		character_->SetPosition({ currentPosition.x, 0.0f, currentPosition.z });
	}
}

void CharacterStateStun::HandleStateTransitionBasedOnReaction()
{
	//リアクションタイプに基づいて状態遷移を処理
	if (currentReactionType_ == ReactionType::kFlinch || !isStunActive_)
	{
		HandleStateTransition();
	}
	else if (currentReactionType_ == ReactionType::kKnockback)
	{
		//スタンを解除し立ち上がるアニメーションに遷移
		isStunActive_ = false;
		character_->GetAnimator()->SetIsBlending(true);
		character_->GetAnimator()->SetBlendDuration(1.0f);
		SetAnimationControllerAndPlayAnimation("StandUp");
	}
}