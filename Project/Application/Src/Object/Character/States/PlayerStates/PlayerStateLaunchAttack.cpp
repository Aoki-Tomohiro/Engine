#include "PlayerStateLaunchAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateRoot.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateAttack.h"
#include "Application/Src/Object/Weapon/Weapon.h"

void PlayerStateLaunchAttack::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションの状態を取得して設定
	animationState_ = player_->GetCombatAnimationEditor()->GetAnimationState("LaunchAttack");

	//アニメーションを再生
	player_->GetAnimator()->PlayAnimation("LaunchAttack", 2.0f, false);

	//打ち上げ攻撃のフラグを立てる
	player_->SetActionFlag(Player::ActionFlag::kLaunchAttack, true);

	//武器にパラメーターを設定
	player_->ApplyParametersToWeapon("PlayerWeapon", animationState_.phases[phaseIndex_]);
}

void PlayerStateLaunchAttack::Update()
{
	//攻撃状態への遷移要求をチェック
	CheckForAttackStateTransition();

	//武器を取得
	Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>("PlayerWeapon");

	//アニメーターを取得
	AnimatorComponent* animator = player_->GetAnimator();

	//現在のアニメーション時間を取得
	float currentAnimationTime = animator->GetIsBlendingCompleted() ? animator->GetCurrentAnimationTime() : animator->GetNextAnimationTime();

	//アニメーションのフェーズを更新
	UpdateAnimationPhase(weapon, currentAnimationTime);

	//アニメーションのフェーズごとの処理
	HandleCurrentPhase(weapon);
}

void PlayerStateLaunchAttack::OnCollision(GameObject* other)
{
	//衝突処理
	player_->ProcessCollisionImpact(other, false);
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
		//フェーズを進める
		phaseIndex_++;
		//ヒットタイマーをリセット
		hitTimer_ = 0.0f;
		//ヒットカウントをリセット
		hitCount_ = 0;
		//武器に新しいフェーズのパラメータを適用
		player_->ApplyParametersToWeapon("PlayerWeapon", animationState_.phases[phaseIndex_]);
		//攻撃状態を無効にする
		weapon->SetIsAttack(false);

		//攻撃状態になったらパーティクルを生成
		if (animationState_.phases[phaseIndex_].name == "NormalAttack")
		{
			//エミッターと加速フィールドのオフセット
			Vector3 emitterAndAccelerationFieldPosition = player_->GetPosition() + Mathf::RotateVector(launchParticleOffset_, player_->GetDestinationQuaternion());

			//パーティクルの生成
			CreateLaunchParticles(emitterAndAccelerationFieldPosition);

			//加速フィールドの生成
			CreateAccelerationField(emitterAndAccelerationFieldPosition);
		}
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
	if (Mathf::Length(GetInputValue()) > player_->GetRootParameters().walkThreshold || player_->GetAnimator()->GetIsAnimationFinished())
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

void PlayerStateLaunchAttack::CreateLaunchParticles(const Vector3& emitterPosition)
{
	//速度を計算
	Vector3 velocity = Mathf::RotateVector(launchParticleVelocity_, player_->GetDestinationQuaternion());

	//エミッターの生成
	ParticleEmitter* emitter = EmitterBuilder().SetColor({ 1.0f, 0.2f, 0.2f, 1.0f }, { 1.0f, 0.2f, 0.2f, 1.0f }).SetCount(40)
		.SetEmitterLifeTime(0.4f).SetEmitterName("LaunchAttack").SetFrequency(0.01f).SetLifeTime(0.5f, 0.7f).SetRadius(1.0f)
		.SetRotate({ 0.0f, 0.0f, -std::numbers::phi_v<float> }, { 0.0f, 0.0f, std::numbers::phi_v<float> }).SetScale({ 1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f })
		.SetTranslation(emitterPosition).SetVelocity(velocity, velocity).Build();
	
	//パーティクルシステムにエミッターを追加
	player_->AddParticleEmitter("Smoke", emitter);
}

void PlayerStateLaunchAttack::CreateAccelerationField(const Vector3& accelerationPosition)
{
	//加速度を計算
	Vector3 acceleration = Mathf::RotateVector(launchParticleAcceleration_, player_->GetDestinationQuaternion());

	//加速フィールドの生成
	AccelerationField* accelerationField = new AccelerationField();
	accelerationField->Initialize("LaunchAttack", 2.0f);
	accelerationField->SetTranslate(accelerationPosition);
	accelerationField->SetAcceleration(acceleration);
	accelerationField->SetMin({ -50.0f,-50.0f,-50.0f });
	accelerationField->SetMax({ 50.0f,100.0f,50.0f });

	//パーティクルシステムに加速フィールドを追加
	player_->AddAccelerationField("Smoke", accelerationField);
}

Vector3 PlayerStateLaunchAttack::GetInputValue()
{
	//スティックの入力を取得
	return { input_->GetLeftStickX(), 0.0f, input_->GetLeftStickY() };
}