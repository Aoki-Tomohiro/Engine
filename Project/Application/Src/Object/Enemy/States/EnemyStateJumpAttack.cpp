#include "EnemyStateJumpAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRoot.h"
#include "Application/Src/Object/Weapon/Weapon.h"

void EnemyStateJumpAttack::Initialize()
{
	//ジャンプ攻撃アニメーションを再生する
	enemy_->PlayAnimation("JumpAttack", 1.0f, false);

	//アニメーションの状態を取得して設定
	animationState_ = enemy_->GetCombatAnimationEditor()->GetAnimationState("JumpAttack");

	//移動開始座標を設定
	startPosition_ = enemy_->GetPosition();

	//移動終了座標を設定
	endPosition_ = GameObjectManager::GetInstance()->GetMutableGameObject<Player>("Player")->GetHipWorldPosition();
	endPosition_.y = enemy_->GetJumpAttackParameters().heightOffset_;

	//武器に初期パラメータを適用
	enemy_->ApplyParametersToWeapon(animationState_.phases[phaseIndex_]);
}

void EnemyStateJumpAttack::Update()
{
	//武器と現在のアニメーション時間を取得
	Weapon* weapon = GameObjectManager::GetInstance()->GetMutableGameObject<Weapon>("EnemyWeapon");
	float currentAnimationTime = enemy_->GetIsBlendingCompleted() ? enemy_->GetCurrentAnimationTime() : enemy_->GetNextAnimationTime();

	//アニメーションのフェーズを更新
	UpdateAnimationPhase(weapon, currentAnimationTime);

	//アニメーションのフェーズごとの処理
	HandleCurrentPhase(weapon);

	//アニメーションが終了した場合
	if (enemy_->GetIsAnimationFinished())
	{
		//敵の位置をアニメーション後の位置に補正
		enemy_->SetIsAnimationCorrectionActive(true);
		enemy_->SetPosition(enemy_->GetPosition() + Vector3(enemy_->GetHipLocalPosition().x, 0.0f, enemy_->GetHipLocalPosition().z));

		//アニメーションを停止
		enemy_->StopAnimation();

		//通常状態に戻す
		enemy_->ChangeState(new EnemyStateRoot());
	}
}

void EnemyStateJumpAttack::OnCollision(GameObject* other)
{
	//衝突処理
	enemy_->ProcessCollisionImpact(other, false);
}

void EnemyStateJumpAttack::UpdateAnimationPhase(Weapon* weapon, float currentAnimationTime)
{
	//現在のアニメーション時間がフェーズの持続時間を超えた場合、次のフェーズに進む
	if (phaseIndex_ < animationState_.phases.size() - 1 && currentAnimationTime > animationState_.phases[phaseIndex_].duration)
	{
		//攻撃状態を無効にする
		weapon->SetIsAttack(false);

		//フェーズを進め、速度を再計算
		phaseIndex_++;
		UpdateVelocityForCurrentPhase();
		enemy_->ApplyParametersToWeapon(animationState_.phases[phaseIndex_]);
	}
}

void EnemyStateJumpAttack::UpdateVelocityForCurrentPhase()
{
	//現在のフェーズに基づいて速度を更新
	velocity_ = { 0.0f,animationState_.phases[phaseIndex_].attackSettings.moveSpeed ,0.0f };
}

void EnemyStateJumpAttack::HandleCurrentPhase(Weapon* weapon)
{
	//現在のフェーズに応じて適切な処理を実行
	switch (phaseIndex_)
	{
	case kCharge:
		//チャージフェーズの処理
		ChargeUpdate();
		break;
	case kAttack:
		//攻撃フェーズの処理
		AttackUpdate(weapon);
		break;
	case kRecovery:
		//攻撃の判定をなくす
		weapon->SetIsAttack(false);
		break;
	}
}

void EnemyStateJumpAttack::ChargeUpdate()
{
	//現在のアニメーション時間を取得
	float currentAnimationTime = enemy_->GetIsBlendingCompleted() ? enemy_->GetCurrentAnimationTime() : enemy_->GetNextAnimationTime();

	//イージング係数を計算
	float easingParameter = std::min<float>(1.0f, currentAnimationTime / animationState_.phases[0].duration);

	//イージングさせる
	Vector3 easedPosition = startPosition_ + (endPosition_ - startPosition_) * Mathf::EaseInCubic(easingParameter);

	//座標を設定
	enemy_->SetPosition(easedPosition);
}

void EnemyStateJumpAttack::AttackUpdate(Weapon* weapon)
{
	//移動処理
	enemy_->Move(velocity_);

	//地面に埋まらないようにする
	Vector3 movedPosition = enemy_->GetPosition();
	if (movedPosition.y <= 0.0f)
	{
		movedPosition.y = 0.0f;
		enemy_->SetPosition(movedPosition);
	}

	//攻撃がヒットしていない場合
	if (!isHit_)
	{
		//武器の衝突判定を付ける
		weapon->SetIsAttack(true);

		//攻撃が当たっていたら
		if (weapon->GetIsHit())
		{
			isHit_ = true;
			weapon->SetIsAttack(false);
		}
	}
}