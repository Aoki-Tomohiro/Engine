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
	//スタン中のノックバック処理
	if (isStunActive_)
	{
		character_->ApplyKnockback();
	}

	//アニメーションイベントの更新
	UpdateAnimationState();

	//キャラクターの位置を地面に補正
	AdjustPositionToGround();

	//スタン状態からの回復を管理
	HandleStunRecovery();

	//アニメーション終了と状態遷移処理
	if (character_->GetAnimator()->GetIsAnimationFinished() && character_->GetPosition().y == 0.0f && !isStunActive_)
	{
		ManageStandUpAnimationAndTransition();
	}
}

void CharacterStateStun::OnCollision(GameObject* other)
{
	//衝突処理
	character_->ProcessCollisionImpact(other, isStunActive_);
}

void CharacterStateStun::PlayReactionAnimation(const ReactionType reactionType)
{
	//リアクションタイプに応じたアニメーションのマッピング
	static const std::unordered_map<ReactionType, std::string> reactionAnimations = {
		{ReactionType::kFront, "ReactFront"},   //前方リアクション
		{ReactionType::kBack, "ReactBack"},     //後方リアクション
		{ReactionType::kLeft, "ReactLeft"},     //左側リアクション
		{ReactionType::kRight, "ReactRight"},   //右側リアクション
		{ReactionType::kKnockback, "Knockback"} //吹き飛ばし
	};

	//該当するアニメーションを再生
	auto it = reactionAnimations.find(reactionType);
	if (it != reactionAnimations.end())
	{
		SetAnimationControllerAndPlayAnimation(it->second);
	}
}

void CharacterStateStun::AdjustPositionToGround()
{
	//キャラクターの現在位置を取得
	Vector3 currentPosition = character_->GetPosition();

	//地面の高さに位置を補正
	if (currentPosition.y < 0.0f)
	{
		character_->SetPosition({ currentPosition.x, 0.0f, currentPosition.z });
		character_->GetAnimator()->ResumeAnimation(); //アニメーションの一時停止を解除
	}
	else
	{
		PauseAnimationIfRequired(); //必要に応じてアニメーションを一時停止
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
	if (!isStunActive_ || character_->GetPosition().y != 0.0f) return;

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
		isPlayStundupAnimation_ = true; //立ち上がりアニメーション再生のフラグを立てる
		character_->GetAnimator()->SetIsBlending(true); //ブレンドを有効化
		character_->GetAnimator()->SetBlendDuration(kStandUpBlendDuration); //ブレンド時間を設定
		SetAnimationControllerAndPlayAnimation("StandUp"); //立ち上がりアニメーションを再生
		return;
	}

	//次の状態へ遷移
	HandleStateTransition();
}