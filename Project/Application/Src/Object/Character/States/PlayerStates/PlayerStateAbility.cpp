#include "PlayerStateAbility.h"
#include "Application/Src/Object/Character/Player/Player.h"

void PlayerStateAbility::Initialize()
{
	//アビリティの名前の取得とフラグを初期化
	abilityName_ = GetAbilityNameAndResetFlag();

	//アビリティのアニメーションの再生とアニメーションコントローラーを取得
	SetAnimationControllerAndPlayAnimation(abilityName_);
}

void PlayerStateAbility::Update()
{
	//アニメーションイベントを実行
	UpdateAnimationState();

	//状態遷移が発生していた場合はクールダウンをリセットする
	if (HasStateTransitioned())
	{
		GetPlayer()->ResetCooldown(abilityName_);
	}

	//アニメーションが終了していた場合アビリティのクールダウンをリセットし状態遷移する
	if (character_->GetAnimator()->GetIsAnimationFinished())
	{
		GetPlayer()->ResetCooldown(abilityName_);
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
	//プレイヤーを取得
	Player* player = GetPlayer();

	//1番目または2番目のアビリティをフラグに基づいて取得
	if (player->GetActionFlag(Player::ActionFlag::kAbility1Enabled))
	{
		player->SetActionFlag(Player::ActionFlag::kAbility1Enabled, false);
		return player->GetAbilityName(false);  //1番目のアビリティ
	}
	else if (player->GetActionFlag(Player::ActionFlag::kAbility2Enabled))
	{
		player->SetActionFlag(Player::ActionFlag::kAbility2Enabled, false);
		return player->GetAbilityName(true);   //2番目のアビリティ
	}
	return ""; //フラグが立っていない場合は空文字
}