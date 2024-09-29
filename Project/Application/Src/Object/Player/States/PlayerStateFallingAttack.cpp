#include "PlayerStateFallingAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Laser/Laser.h"
#include "Application/Src/Object/Pillar/Pillar.h"

void PlayerStateFallingAttack::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションの状態を取得して設定
	animationState_ = player_->GetCombatAnimationEditor()->GetAnimationState("FallingAttack");

	//アニメーションを再生
	player_->PlayAnimation("FallingAttack", 2.0f, false);

	//アニメーション時間を設定
	player_->SetNextAnimationTime(animationState_.phases[phaseIndex_].duration);

	//落下攻撃のフラグを立てる
	player_->SetActionFlag(Player::ActionFlag::kFallingAttack, true);

	//武器にパラメーターを設定
	ApplyParametersToWeapon();
}

void PlayerStateFallingAttack::Update()
{
	//アニメーションによる座標のずれを修正
	player_->CorrectAnimationOffset();

	//武器を取得
	Weapon* weapon = GameObjectManager::GetInstance()->GetMutableGameObject<Weapon>("PlayerWeapon");

	//現在のアニメーション時間を取得
	float currentAnimationTime = GetCurrentAnimationTime();

	//アニメーションのフェーズを更新
	UpdateAnimationPhase(weapon, currentAnimationTime);

	//フェーズごとの更新
	HandleCurrentPhase(weapon);

	//アニメーションが終了していた場合
	if (player_->GetIsAnimationFinished() || isRecoveryCanceled_)
	{
		//落下攻撃終了
		EndFallingAttack();
	}
}

const float PlayerStateFallingAttack::GetCurrentAnimationTime() const
{
	return player_->GetIsBlendingCompleted() ? player_->GetCurrentAnimationTime() : player_->GetNextAnimationTime();
}

void PlayerStateFallingAttack::EndFallingAttack()
{
	//落下攻撃のフラグを折る
	player_->SetActionFlag(Player::ActionFlag::kFallingAttack, false);

	//プレイヤーの位置をアニメーション後の位置に補正
	CorrectPlayerPosition();

	//アニメーションを停止
	player_->StopAnimation();

	//通常状態に戻す
	player_->ChangeState(new PlayerStateRoot());
}

void PlayerStateFallingAttack::CorrectPlayerPosition()
{
	//プレイヤーの位置をアニメーション後の位置に補正
	player_->SetIsAnimationCorrectionActive(true);
	player_->SetPosition(player_->GetPosition() + Vector3(player_->GetHipLocalPosition().x, 0.0f, player_->GetHipLocalPosition().z));
}

void PlayerStateFallingAttack::OnCollision(GameObject* other)
{
	//衝突相手が武器だった場合
	if (Weapon* weapon = dynamic_cast<Weapon*>(other))
	{
		//プレイヤーの位置をアニメーション後の位置に補正
		CorrectPlayerPosition();

		//アニメーションを停止
		player_->StopAnimation();

		//ダメージを食らった処理を実行
		player_->HandleIncomingDamage(weapon->GetKnockbackSettings(), weapon->GetDamage(), true);
	}
	else if (Laser* laser = dynamic_cast<Laser*>(other))
	{
		//プレイヤーの位置をアニメーション後の位置に補正
		CorrectPlayerPosition();

		//アニメーションを停止
		player_->StopAnimation();

		//ダメージを食らった処理を実行
		player_->HandleIncomingDamage(KnockbackSettings{}, laser->GetDamage(), true);
	}
	else if (Pillar* pillar = dynamic_cast<Pillar*>(other))
	{
		//プレイヤーの位置をアニメーション後の位置に補正
		CorrectPlayerPosition();

		//アニメーションを停止
		player_->StopAnimation();

		//ダメージを食らった処理を実行
		player_->HandleIncomingDamage(KnockbackSettings{}, pillar->GetDamage(), true);
	}
}

void PlayerStateFallingAttack::ApplyParametersToWeapon()
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

void PlayerStateFallingAttack::UpdateAnimationPhase(Weapon* weapon, float currentAnimationTime)
{
	//次のフェーズがあり現在のアニメーション時間が現在のフェーズの持続時間を超えた場合フェーズを進める
	if (phaseIndex_ < animationState_.phases.size() - 1 && currentAnimationTime >= animationState_.phases[phaseIndex_].duration)
	{
		//フェーズを進める
		phaseIndex_++;
		//武器に新しいフェーズのパラメータを適用
		ApplyParametersToWeapon();
		//フェーズ遷移処理
		HandlePhaseTransition(weapon);
	}
}

void PlayerStateFallingAttack::HandlePhaseTransition(Weapon* weapon)
{
	//攻撃フェーズに移行した場合
	if (animationState_.phases[phaseIndex_].name == "Attack")
	{
		//攻撃状態を有効にする
		weapon->SetIsAttack(true);

		//アニメーションを一時停止
		player_->PauseAnimation();

		//移動開始座標を設定
		startPosition_ = player_->GetPosition();

		//移動終了座標を設定
		endPosition_ = startPosition_;
		endPosition_.y = 0.0f;
	}
	else
	{
		//攻撃状態を無効にする
		weapon->SetIsAttack(false);

		//アニメーション停止を解除
		player_->ResumeAnimation();
	}
}

void PlayerStateFallingAttack::HandleCurrentPhase(Weapon* weapon)
{
	//現在のフェーズに応じて適切な処理を実行
	switch (phaseIndex_)
	{
	case kAttack:
		//攻撃フェーズの処理
		AttackUpdate(weapon);
		break;
	case kRecovery:
		//溜めフェーズの処理
		RecoveryUpdate();
		break;
	}
}

void PlayerStateFallingAttack::AttackUpdate(Weapon* weapon)
{
	//イージング係数を加算
	easingParameter_ += GameTimer::GetDeltaTime() * 4.0f;
	easingParameter_ = std::clamp(easingParameter_, 0.0f, 1.0f);

	//移動処理
	Vector3 easedPosition = startPosition_ + (endPosition_ - startPosition_) * Mathf::EaseInCubic(easingParameter_);
	player_->SetPosition(easedPosition);

	//アニメーション時間を設定
	float currentAnimationTime = Mathf::Lerp(animationState_.phases[1].duration, animationState_.phases[2].duration, easingParameter_);
	player_->SetCurrentAnimationTime(currentAnimationTime);

	//武器が敵にヒットしたかをチェック
	if (weapon->GetIsHit())
	{
		weapon->SetIsAttack(false);
	}
}

void PlayerStateFallingAttack::RecoveryUpdate()
{
	//スティックの入力を取得
	Vector3 inputValue = {
		input_->GetLeftStickX(),
		0.0f,
		input_->GetLeftStickY(),
	};

	//入力値が歩行の閾値を超えている場合は硬直をキャンセルする
	if (Mathf::Length(inputValue) > player_->GetRootParameters().walkThreshold)
	{
		isRecoveryCanceled_ = true;
	}
}