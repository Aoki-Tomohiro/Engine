#include "PlayerStateFallingAttack.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"

void PlayerStateFallingAttack::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//落下攻撃のフラグを設定
	player_->SetActionFlag(Player::ActionFlag::kFallingAttack, true);

	//アニメーションの初期化
	player_->SetAnimationName("Armature.001|mixamo.com|Layer0.022");
	player_->SetIsAnimationLoop(false);
	player_->SetAnimationTime(0.0f);
	player_->SetAnimationSpeed(2.0f);

	//状態の取得
	state_ = player_->GetAnimationStateManager()->GetAnimationState("FallingAttack");

	//最初のフェーズの名前を設定
	phaseName_ = state_.phases[0].name;

	//速度を設定
	velocity_.y = player_->GetFallingAttackParameters().fallingSpeed;
}

void PlayerStateFallingAttack::Update()
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
				break; //フェーズが更新されたのでループを抜ける
			}
		}
	}

	//アニメーションのずれを修正
	player_->CorrectAnimationOffset();

	//攻撃振りの状態の場合
	if (phaseName_ == "Swing")
	{
		//移動処理
		velocity_.y += player_->GetFallingAttackParameters().fallingAcceleration;
		player_->Move(velocity_);

		//地面に埋まらないようにする
		Vector3 currentPosition = player_->GetPosition();
		if (currentPosition.y <= 0.0f)
		{
			player_->SetPosition({ currentPosition.x, 0.0f, currentPosition.z });
		}
	}
	else if (phaseName_ == "Recovery")
	{
		//攻撃が当たっていたら
		Weapon* weapon = GameObjectManager::GetInstance()->GetMutableGameObject<Weapon>("PlayerWeapon");
		if (weapon->GetIsHit())
		{
			hasHitTargetThisAttack = true;
		}

		//今回の攻撃が当たっていなかった場合
		if (!hasHitTargetThisAttack)
		{
			//武器の判定を付ける
			weapon->SetIsAttack(true);
		}
		//今回の攻撃が当たっていた場合
		else
		{
			//武器の判定をなくす
			weapon->SetIsAttack(false);
		}
	}

	//アニメーションが終了していた場合
	if (player_->GetIsAnimationEnd())
	{
		//武器の判定をなくす
		Weapon* weapon = GameObjectManager::GetInstance()->GetMutableGameObject<Weapon>("PlayerWeapon");
		weapon->SetIsAttack(false);

		//落下攻撃のフラグを設定
		player_->SetActionFlag(Player::ActionFlag::kFallingAttack, false);

		//アニメーション後の座標を代入
		Vector3 hipPosition = player_->GetHipWorldPosition();
		hipPosition.y = 0.0f;
		player_->SetPosition(hipPosition);

		//通常状態に戻す
		player_->ChangeState(new PlayerStateRoot());
	}
}