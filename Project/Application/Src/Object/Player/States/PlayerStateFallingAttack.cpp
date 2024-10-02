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
	player_->ApplyParametersToWeapon(animationState_.phases[phaseIndex_]);
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
	if (player_->GetIsAnimationFinished())
	{
		//落下攻撃終了
		EndFallingAttack();

		//通常状態に戻す
		player_->ChangeState(new PlayerStateRoot());
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
}

void PlayerStateFallingAttack::CorrectPlayerPosition()
{
	//プレイヤーの位置をアニメーション後の位置に補正
	player_->SetIsAnimationCorrectionActive(true);
	player_->SetPosition(player_->GetPosition() + Vector3(player_->GetHipLocalPosition().x, 0.0f, player_->GetHipLocalPosition().z));
}

void PlayerStateFallingAttack::OnCollision(GameObject* other)
{
	//衝突処理
	if (Weapon* weapon = dynamic_cast<Weapon*>(other))
	{
		ApplyDamageAndKnockback(weapon->GetKnockbackSettings(), weapon->GetDamage());
	}
	else if (Laser* laser = dynamic_cast<Laser*>(other))
	{
		ApplyDamageAndKnockback(KnockbackSettings{}, laser->GetDamage());
	}
	else if (Pillar* pillar = dynamic_cast<Pillar*>(other))
	{
		ApplyDamageAndKnockback(KnockbackSettings{}, pillar->GetDamage());
	}
}

void PlayerStateFallingAttack::UpdateAnimationPhase(Weapon* weapon, float currentAnimationTime)
{
	//次のフェーズがあり現在のアニメーション時間が現在のフェーズの持続時間を超えた場合フェーズを進める
	if (phaseIndex_ < animationState_.phases.size() - 1 && currentAnimationTime >= animationState_.phases[phaseIndex_].duration)
	{
		//フェーズを進める
		phaseIndex_++;
		//ヒットカウントをリセット
		hitCount_ = 0;
		//ヒットタイマーをリセット
		hitTimer_ = 0.0f;
		//武器に新しいフェーズのパラメータを適用
		player_->ApplyParametersToWeapon(animationState_.phases[phaseIndex_]);
		//フェーズ遷移処理
		HandlePhaseTransition(weapon);
	}
}

void PlayerStateFallingAttack::HandlePhaseTransition(Weapon* weapon)
{
	//攻撃フェーズに移行した場合
	if (animationState_.phases[phaseIndex_].name == "Attack")
	{
		//移動開始座標を設定
		startPosition_ = player_->GetPosition();

		//移動終了座標を設定
		endPosition_ = startPosition_;
		endPosition_.y = 0.0f;

		//攻撃状態を有効にする
		weapon->SetIsAttack(true);

		//アニメーションを一時停止
		player_->PauseAnimation();
	}
	else if(animationState_.phases[phaseIndex_].name == "SlamAttack")
	{
		//攻撃状態を設定
		weapon->SetIsAttack(true);

		//アニメーション停止を解除
		player_->ResumeAnimation();

		//パーティクルを生成
		CreateSlamAttackParticles();
	}
	else
	{
		//攻撃状態を設定
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
	case kSlamAttack:
		//武器のヒット判定処理
		HandleWeaponHit(weapon);
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

	//武器のヒット判定処理
	HandleWeaponHit(weapon);
}

void PlayerStateFallingAttack::HandleWeaponHit(Weapon* weapon)
{
	//現在のフェーズでのヒット数が上限に達していない場合
	if (hitCount_ < animationState_.phases[phaseIndex_].attackSettings.hitCount)
	{
		//ヒットタイマーを進める
		hitTimer_ += GameTimer::GetDeltaTime();

		//ヒットタイマーがヒット間隔を超えた場合、攻撃判定を付ける
		if (hitTimer_ > animationState_.phases[phaseIndex_].attackSettings.hitInterval)
		{
			hitTimer_ = 0.0f;
			weapon->SetIsAttack(true);
		}
		else
		{
			weapon->SetIsAttack(false);
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

void PlayerStateFallingAttack::ApplyDamageAndKnockback(const KnockbackSettings& knockbackSettings, const float damage)
{
	//武器の当たり判定をなくす
	Weapon* playerWeapon = GameObjectManager::GetInstance()->GetMutableGameObject<Weapon>("PlayerWeapon");
	playerWeapon->SetIsAttack(false);

	//落下攻撃終了
	EndFallingAttack();

	//ダメージを食らった処理を実行
	player_->HandleIncomingDamage(knockbackSettings, damage, true);
}

void PlayerStateFallingAttack::CreateSlamAttackParticles()
{
	//パーティクルを出す
	for (int32_t i = 0; i <= 360; ++i)
	{
		//Quaternionを計算
		Quaternion rotation = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f, 1.0f, 0.0f }, static_cast<float>(i) * (std::numbers::pi_v<float> / 180.0f));

		//速度を計算
		const float kMoveSpeed = 0.8f;
		Vector3 velocity = Mathf::Normalize(Mathf::RotateVector({ 0.0f,0.0f,1.0f }, rotation)) * kMoveSpeed;

		//エミッターの生成
		ParticleEmitter* emitter = EmitterBuilder()
			.SetEmitterName("SlamAttack").SetColor({ 1.0f, 0.1f, 0.0f, 1.0f }, { 1.0f, 0.1f, 0.0f, 1.0f })
			.SetEmitterLifeTime(0.0f).SetCount(1).SetFrequency(100.0f).SetLifeTime(0.6f, 0.8f).SetRadius(0.0f)
			.SetRotate({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }).SetScale({ 1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f })
			.SetTranslation(player_->GetHipWorldPosition()).SetVelocity(velocity, velocity).Build();

		//パーティクルシステムにエミッターを追加
		player_->AddParticleEmitter("Smoke", emitter);
	}
}