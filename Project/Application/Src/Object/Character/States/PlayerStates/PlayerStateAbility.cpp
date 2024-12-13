/**
 * @file PlayerStateAbility.cpp
 * @brief プレイヤーのアビリティ状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#include "PlayerStateAbility.h"
#include "Application/Src/Object/Character/Player/Player.h"

void PlayerStateAbility::Initialize()
{
	//アビリティの名前の取得とフラグを初期化
	abilityName_ = GetAbilityNameAndResetFlag();

	//攻撃フラグを立てる
	GetPlayer()->SetActionFlag(Player::ActionFlag::kIsAttacking, true);

	//アビリティのアニメーションの再生とアニメーションコントローラーを取得
	SetAnimationControllerAndPlayAnimation(abilityName_);
}

void PlayerStateAbility::Update()
{
	//アニメーションイベントを実行
	UpdateAnimationState();

	//状態遷移が発生していた場合
	if (HasStateTransitioned())
	{
		//アビリティのクールダウンをリセット
		GetPlayer()->ResetCooldown(abilityName_);
		//攻撃フラグをリセット
		GetPlayer()->SetActionFlag(Player::ActionFlag::kIsAttacking, false);
	}

	//アニメーションが終了していた場合
	if (character_->GetAnimator()->GetIsAnimationFinished())
	{
		//攻撃フラグをリセット
		GetPlayer()->SetActionFlag(Player::ActionFlag::kIsAttacking, false);
		//アビリティのクールダウンをリセット
		GetPlayer()->ResetCooldown(abilityName_);
		//デフォルトの状態に遷移
		HandleStateTransition();
	}
}

void PlayerStateAbility::OnCollision(GameObject* other)
{
	//衝突処理
	character_->ProcessCollisionImpact(other, false);
}

const std::string PlayerStateAbility::GetAbilityNameAndResetFlag() const
{
	//最初のアビリティかどうかを判別
	bool isSecond = abilityIndex_ != 1;
	//アビリティの名前を返す
	return GetPlayer()->GetAbilityName(isSecond);
}