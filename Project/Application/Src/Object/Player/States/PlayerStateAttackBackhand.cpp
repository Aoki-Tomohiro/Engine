#include "PlayerStateAttackBackhand.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Weapon/Weapon.h"

void PlayerStateAttackBackhand::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションの初期化
	player_->SetAnimationName("Armature.001|mixamo.com|Layer0.023");
	player_->SetIsAnimationLoop(false);
	player_->SetAnimationSpeed(2.0f);
	player_->SetAnimationTime(0.0f);

	//フラグを立てる
	player_->SetActionFlag(Player::ActionFlag::kBackhandAttack, true);

	//状態の取得
	state_ = player_->GetAnimationStateManager()->GetAnimationState("BackhandAttack");
	//最初のフェーズの名前を設定
	phaseName_ = state_.phases[0].name;
}

void PlayerStateAttackBackhand::Update()
{
	//現在のアニメーションの時間を取得
	float animationTime = player_->GetAnimationTime();

	//状態の更新
	for (uint32_t i = 0; i < state_.phases.size() - 1; ++i)
	{
		if (phaseName_ == state_.phases[i].name)
		{
			if (animationTime >= state_.phases[i].animationTime)
			{
				phaseName_ = state_.phases[i + 1].name;
			}
		}
	}

	//武器を取得
	Weapon* weapon = GameObjectManager::GetInstance()->GetMutableGameObject<Weapon>("PlayerWeapon");

	if (phaseName_ == "Swing")
	{
		//ヒットタイマーを進める
		hitTimer_ += GameTimer::GetDeltaTime();

		//ヒットカウントが最大値を超えていない場合
		if (hitCount_ < player_->GetBackhandAttackParameters().kMaxHitCount)
		{
			//ヒットタイマーが一定値を超えていた場合
			if (hitTimer_ >= player_->GetBackhandAttackParameters().hitInterval)
			{
				//フラグとタイマーをリセット
				hitTimer_ = 0.0f;
				hitCount_++;

				//当たり判定を付ける
				weapon->SetIsAttack(true);
			}
			else
			{
				weapon->SetIsAttack(false);
			}
		}
		else
		{
			weapon->SetIsAttack(false);
		}
	}
	else
	{
		weapon->SetIsAttack(false);
	}

	//アニメーションが終了したら通常状態に戻す
	if (player_->GetIsAnimationEnd())
	{
		//フラグをリセット
		player_->SetActionFlag(Player::ActionFlag::kBackhandAttack, false);

		//通常状態に戻す
		player_->ChangeState(new PlayerStateRoot());
	}
}