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

	//ノックバックの逆方向に回転させる
	character_->Rotate(Mathf::Normalize(Vector3{ character_->GetKnockbackParameters().velocity.x, 0.0f, character_->GetKnockbackParameters().velocity.z }) * -1.0f);

	//アニメーションブレンドを無効にする
	character_->GetAnimator()->SetIsBlending(false);

	//現在のリアクションタイプを取得
	currentReactionType_ = character_->GetPosition().y == character_->GetAdjustGroundLevel() ? character_->GetKnockbackParameters().reactionType : ReactionType::kKnockback;

	//現在のリアクションタイプがKnockbackの場合は吹き飛ばされている状態にする
	character_->SetIsKnockback(currentReactionType_ == ReactionType::kKnockback);

	//武器のトレイルをなくす
	character_->GetWeapon()->SetIsTrailActive(false);
	
	//武器の判定をなくす
	character_->GetWeapon()->SetIsAttack(false);

	//アニメーションの再生とアニメーションコントローラーを取得
	PlayReactionAnimation(currentReactionType_, character_->GetKnockbackParameters().attackDirection);
}

void CharacterStateStun::Update()
{
	//スタン中のノックバック処理
	if (isStunActive_)
	{
		character_->ApplyKnockback();
	}

	//アニメーションイベントの更新
	UpdateAnimationState();

	//着地状態に関連するアニメーションの管理
	ManageGroundLevelAnimation();

	//スタン状態からの回復を管理
	HandleStunRecovery();

	//アニメーション終了と状態遷移処理
	if (character_->GetAnimator()->GetIsAnimationFinished() && character_->GetPosition().y <= character_->GetAdjustGroundLevel() && !isStunActive_)
	{
		ManageStandUpAnimationAndTransition();
	}
}

void CharacterStateStun::OnCollision(GameObject* other)
{
	//衝突処理
	character_->ProcessCollisionImpact(other, isStunActive_);
}

void CharacterStateStun::PlayReactionAnimation(const ReactionType reactionType, const AttackDirection attackDirection)
{
	//リアクションの種類を設定
	std::string type = reactionType == ReactionType::kFlinch ? "Small" : "Large";

	//攻撃の方向
	switch (attackDirection)
	{
	case AttackDirection::kFront:
		SetAnimationControllerAndPlayAnimation("React" + type + "Front");
		break;
	case AttackDirection::kBack:
		SetAnimationControllerAndPlayAnimation("React" + type + "Back");
		break;
	case AttackDirection::kLeft:
		SetAnimationControllerAndPlayAnimation("React" + type + "Right");
		break;
	case AttackDirection::kRight:
		SetAnimationControllerAndPlayAnimation("React" + type + "Left");
		break;
	}
}

void CharacterStateStun::ManageGroundLevelAnimation()
{
	//アニメーションの一時停止を解除
	if (character_->GetPosition().y < character_->GetAdjustGroundLevel())
	{
		character_->GetAnimator()->ResumeAnimation(); 
	}
	//必要に応じてアニメーションを一時停止
	else
	{
		PauseAnimationIfRequired();
	}
}

void CharacterStateStun::PauseAnimationIfRequired()
{
	//アニメーターを取得
	AnimatorComponent* animator = character_->GetAnimator();

	//現在のアニメーション時間を取得
	float currentAnimationTime = animator->GetIsBlendingCompleted() ? animator->GetCurrentAnimationTime() : animator->GetNextAnimationTime();

	//一定時間を超えている場合はアニメーションを一時停止
	if (currentAnimationTime > knockbackAnimationPauseTime_ && isStunActive_)
	{
		animator->PauseAnimation();
	}
}

void CharacterStateStun::HandleStunRecovery()
{
	//スタン解除や地面にいない場合の処理を飛ばす
	if (!isStunActive_ || character_->GetPosition().y > character_->GetAdjustGroundLevel()) return;

	//アニメーターを取得
	AnimatorComponent* animator = character_->GetAnimator();

	//現在のアニメーション時間を取得
	float currentAnimationTime = animator->GetIsBlendingCompleted() ? animator->GetCurrentAnimationTime() : animator->GetNextAnimationTime();

	//リアクションタイプに基づいてスタン状態を更新
	if (currentReactionType_ != ReactionType::kKnockback)
	{
		//アニメーション終了時にスタン状態を解除
		if (character_->GetAnimator()->GetIsAnimationFinished())
		{
			isStunActive_ = false;
		}
	}
	else if (currentAnimationTime > knockbackAnimationPauseTime_)
	{
		//スタン解除
		isStunActive_ = false;
	}
}

void CharacterStateStun::ManageStandUpAnimationAndTransition()
{
	//吹き飛ばし状態で立ち上がりアニメーションを再生
	if (currentReactionType_ == ReactionType::kKnockback && !isPlayStundupAnimation_)
	{
		//立ち上がりアニメーションのブレンド時間
		static const float kStandUpBlendDuration = 1.0f;
		//立ち上がりアニメーション再生のフラグを立てる
		isPlayStundupAnimation_ = true; 
		//ブレンドを有効化
		character_->GetAnimator()->SetIsBlending(true); 
		//ブレンド時間を設定
		character_->GetAnimator()->SetBlendDuration(kStandUpBlendDuration); 
		//立ち上がりアニメーションを再生
		SetAnimationControllerAndPlayAnimation("StandUp"); 
		//吹き飛ばされている状態を解除
		character_->SetIsKnockback(false);
		return;
	}

	//次の状態へ遷移
	HandleStateTransition();
}