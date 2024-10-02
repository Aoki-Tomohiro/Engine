#include "PlayerStateLaunchAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Player/States/PlayerStateAttack.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Laser/Laser.h"
#include "Application/Src/Object/Pillar/Pillar.h"

void PlayerStateLaunchAttack::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションの状態を取得して設定
	animationState_ = player_->GetCombatAnimationEditor()->GetAnimationState("LaunchAttack");

	//アニメーションを再生
	player_->PlayAnimation("LaunchAttack", 2.0f, false);

	//打ち上げ攻撃のフラグを立てる
	player_->SetActionFlag(Player::ActionFlag::kLaunchAttack, true);

	//武器にヒットストップの持続時間を設定
	ApplyParametersToWeapon();
}

void PlayerStateLaunchAttack::Update()
{
	//攻撃状態への遷移要求をチェック
	CheckForAttackStateTransition();

	//武器を取得
	Weapon* weapon = GameObjectManager::GetInstance()->GetMutableGameObject<Weapon>("PlayerWeapon");

	//現在のアニメーション時間を取得
	float currentAnimationTime = player_->GetIsBlendingCompleted() ? player_->GetCurrentAnimationTime() : player_->GetNextAnimationTime();

	//アニメーションのフェーズを更新
	UpdateAnimationPhase(weapon, currentAnimationTime);

	//アニメーションのフェーズごとの処理
	HandleCurrentPhase(weapon);
}

void PlayerStateLaunchAttack::OnCollision(GameObject* other)
{
	//衝突相手が武器だった場合
	if (Weapon* weapon = dynamic_cast<Weapon*>(other))
	{
		//ダメージを食らった処理を実行
		player_->HandleIncomingDamage(weapon->GetKnockbackSettings(), weapon->GetDamage(), false);
	}
	else if (Laser* laser = dynamic_cast<Laser*>(other))
	{
		//ダメージを食らった処理を実行
		player_->HandleIncomingDamage(KnockbackSettings{}, laser->GetDamage(), false);
	}
	else if (Pillar* pillar = dynamic_cast<Pillar*>(other))
	{
		//ダメージを食らった処理を実行
		player_->HandleIncomingDamage(KnockbackSettings{}, pillar->GetDamage(), false);
	}
}

void PlayerStateLaunchAttack::ApplyParametersToWeapon()
{
	//武器を取得
	Weapon* weapon = GameObjectManager::GetInstance()->GetMutableGameObject<Weapon>("PlayerWeapon");

	//現在のフェーズの設定を取得
	const auto& currentPhase = animationState_.phases[phaseIndex_];

	//ダメージを設定
	weapon->SetDamage(currentPhase.attackSettings.damage);

	//ヒットボックスを設定
	weapon->SetHitbox(currentPhase.hitbox);

	//エフェクトの設定を武器に設定
	weapon->SetEffectSettings(currentPhase.effectSettings);

	//ノックバックの設定を武器に設定
	KnockbackSettings knockbackSettings = currentPhase.knockbackSettings;
	knockbackSettings.knockbackVelocity = Mathf::RotateVector(knockbackSettings.knockbackVelocity, player_->GetDestinationQuaternion());
	knockbackSettings.knockbackAcceleration = Mathf::RotateVector(knockbackSettings.knockbackAcceleration, player_->GetDestinationQuaternion());
	weapon->SetKnockbackSettings(knockbackSettings);
}

void PlayerStateLaunchAttack::CheckForAttackStateTransition()
{
	//Xボタンが押されている場合攻撃状態への遷移要求を設定
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
	{
		isAttackStateTransitionRequested_ = true;
	}
}

void PlayerStateLaunchAttack::UpdateAnimationPhase(Weapon* weapon, float currentAnimationTime)
{
	//次のフェーズがあり現在のアニメーション時間が現在のフェーズの持続時間を超えた場合フェーズを進める
	if (phaseIndex_ < animationState_.phases.size() - 1 && currentAnimationTime > animationState_.phases[phaseIndex_].duration)
	{
		//攻撃状態を無効にする
		weapon->SetIsAttack(false);
		//フェーズを進める
		phaseIndex_++;
		//ヒットタイマーをリセット
		hitTimer_ = 0.0f;
		//ヒットカウントをリセット
		hitCount_ = 0;
		//武器に新しいフェーズのパラメータを適用
		ApplyParametersToWeapon();
	}
}

void PlayerStateLaunchAttack::HandleCurrentPhase(Weapon* weapon)
{
	//現在のフェーズに応じて適切な処理を実行
	switch (phaseIndex_)
	{
	case kCharge:
		break;

	case kFirstAttack:
	case kNormalAttack:
		//攻撃フェーズの処理
		AttackUpdate(weapon);
		break;
	case kRecovery:
		//硬直フェーズの処理
		RecoveryUpdate(weapon);
		break;
	}
}

void PlayerStateLaunchAttack::AttackUpdate(Weapon* weapon)
{
	//軌跡エフェクトを有効にする
	weapon->SetIsTrailActive(true);

	//現在のフェーズの攻撃設定を取得
	const auto& attackSettings = animationState_.phases[phaseIndex_].attackSettings;

	//現在のフェーズでのヒット数が上限に達していない場合
	if (hitCount_ < attackSettings.hitCount)
	{
		//ヒットタイマーを進める
		hitTimer_ += GameTimer::GetDeltaTime();

		//ヒット間隔に基づいて攻撃判定を設定
		if (hitTimer_ > attackSettings.hitInterval)
		{
			hitTimer_ = 0.0f; //ヒットタイマーをリセット
			weapon->SetIsAttack(true); //攻撃状態を有効にする
		}
		else
		{
			weapon->SetIsAttack(false); //攻撃状態を無効にする
		}

		//武器が敵にヒットしたかをチェック
		if (weapon->GetIsHit())
		{
			hitCount_++; //ヒットカウントを増加
		}
	}
	else
	{
		//ヒット数が上限に達した場合攻撃状態を無効にする
		weapon->SetIsAttack(false);
	}
}

void PlayerStateLaunchAttack::RecoveryUpdate(Weapon* weapon)
{
	//攻撃状態を無効にする
	weapon->SetIsAttack(false); 
	//軌跡エフェクトを無効にする
	weapon->SetIsTrailActive(false);

	//打ち上げ攻撃のフラグを折る
	player_->SetActionFlag(Player::ActionFlag::kLaunchAttack, false);

	//プレイヤーの移動入力が閾値を超えている、またはアニメーションが終了した場合通常状態に遷移
	if (Mathf::Length(GetInputValue()) > player_->GetRootParameters().walkThreshold || player_->GetIsAnimationFinished())
	{
		//スキルのクールダウンをリセット
		player_->ResetCooldown(Skill::kLaunchAttack);
		player_->ChangeState(new PlayerStateRoot());
	}
	//攻撃状態への遷移フラグが立っている場合
	else if (isAttackStateTransitionRequested_)
	{
		//スキルのクールダウンをリセット
		player_->ResetCooldown(Skill::kLaunchAttack);
		player_->ChangeState(new PlayerStateAttack());
	}
}

Vector3 PlayerStateLaunchAttack::GetInputValue()
{
	//スティックの入力を取得
	return { input_->GetLeftStickX(), 0.0f, input_->GetLeftStickY() };
}