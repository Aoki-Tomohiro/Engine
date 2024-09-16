#include "EnemyStateRoot.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateTackle.h"
#include "Application/Src/Object/Enemy/States/EnemyStateJumpAttack.h"
#include "Application/Src/Object/Enemy/States/EnemyStateStun.h"
#include "Application/Src/Object/Enemy/States/EnemyStateComboAttack.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/MagicProjectile/MagicProjectile.h"

//初期化される前のアクションのインデックスを最大値で設定
int EnemyStateRoot::preAction_ = kMaxCloseRangeActions;

void EnemyStateRoot::Initialize()
{
	//アニメーションの初期化
	enemy_->PlayAnimation("Idle", 1.0f, true);

	//スタンから復帰した状態でなければ次の行動の間隔をランダムに設定
	if (!enemy_->GetIsStunRecovered())
	{
		currentActionInterval_ = RandomGenerator::GetRandomFloat(enemy_->GetRootParameters().minActionInterval_, enemy_->GetRootParameters().maxActionInterval_);
	}
	//スタンから復帰した状態ならフラグをリセット
	else
	{
		enemy_->SetIsStunRecovered(false);
	}
}

void EnemyStateRoot::Update()
{
	//ゲームが終了していたら処理を飛ばす
	if (enemy_->GetIsGameFinished() || enemy_->GetIsDebug())
	{
		enemy_->PlayAnimation("Idle", 1.0f, true);
		return;
	}

	//アクションタイマーの更新
	actionTimer_ += GameTimer::GetDeltaTime() * enemy_->GetTimeScale();

	//アクションタイマーが経過していた場合
	if (actionTimer_ > currentActionInterval_)
	{
		isActionCooldownComplete = true;
	}

	//プレイヤーの方向に敵を回転させる
	RotateTowardsPlayer();

	//敵の位置とアニメーションの更新
	UpdatePositionAndAnimation(); 
}

void EnemyStateRoot::OnCollision(GameObject* other)
{
	//衝突相手が武器だった場合
	if (Weapon* weapon = dynamic_cast<Weapon*>(other))
	{
		//HPを減らす
		enemy_->SetHP(enemy_->GetHP() - weapon->GetDamage());
		//ノックバックの速度を設定し、スタン状態にする
		enemy_->SetKnockbackSettings(weapon->GetKnockbackSettings());
		enemy_->ChangeState(new EnemyStateStun());
	}
	//衝突相手が魔法だった場合
	else if (MagicProjectile* magicProjectile = dynamic_cast<MagicProjectile*>(other))
	{
		//HPを減らす
		enemy_->SetHP(enemy_->GetHP() - magicProjectile->GetDamage());

		//チャージマジックの場合
		if (magicProjectile->GetMagicType() == MagicProjectile::MagicType::kCharged)
		{
			//ノックバックの速度を設定し、スタン状態にする
			enemy_->SetKnockbackSettings(magicProjectile->GetKnockbackSettings());
			enemy_->ChangeState(new EnemyStateStun());
		}
	}
}

void EnemyStateRoot::UpdatePositionAndAnimation()
{
	//プレイヤーと敵の座標を取得
	Vector3 playerPosition = GameObjectManager::GetInstance()->GetMutableGameObject<Player>("")->GetHipWorldPosition();
	Vector3 enemyPosition = enemy_->GetHipWorldPosition();

	//プレイヤーへの方向ベクトルを計算
	Vector3 directionToPlayer = playerPosition - enemyPosition;
	directionToPlayer.y = 0.0f;

	//敵がプレイヤーから離れている場合、プレイヤーに向かって移動
	if (Mathf::Length(directionToPlayer) > enemy_->GetRootParameters().stopDistance)
	{
		//プレイヤーに向かって移動
		MoveTowardsPlayer(directionToPlayer); 
	}
	else
	{
		//アクションのクールダウンが終了している場合
		if (isActionCooldownComplete)
		{
			//次のアクションを実行
			PerformNextAction();
		}
		else
		{
			//プレイヤーに近づいている場合は待機アニメーションに戻す
			PlayIdleAnimationIfNotPlaying();
		}
	}
}

void EnemyStateRoot::MoveTowardsPlayer(const Vector3& direction)
{
	//移動速度を設定
	float currentMoveSpeed = isActionCooldownComplete ? enemy_->GetRootParameters().runSpeed : enemy_->GetRootParameters().walkSpeed;

	//敵をプレイヤーに向かって移動
	velocity_ = Mathf::Normalize(direction) * currentMoveSpeed;
	enemy_->Move(velocity_);

	//現在のアニメーションに応じて適切なアニメーションを再生
	if (isActionCooldownComplete)
	{
		PlayAnimationIfNotPlaying("Run");
	}
	else
	{
		PlayAnimationIfNotPlaying("Walk");
	}
}

void EnemyStateRoot::PlayIdleAnimationIfNotPlaying()
{
	//現在のアニメーションがIdleでない場合、待機アニメーションを再生
	PlayAnimationIfNotPlaying("Idle");
}

void EnemyStateRoot::PlayAnimationIfNotPlaying(const std::string& animationName)
{
	if (currentAnimation_ != animationName)
	{
		enemy_->PlayAnimation(animationName, 1.0f, true);
		currentAnimation_ = animationName;
	}
}

void EnemyStateRoot::RotateTowardsPlayer()
{
	//プレイヤーへの方向を計算
	Vector3 directionToPlayer = GameObjectManager::GetInstance()->GetMutableGameObject<Player>("")->GetHipWorldPosition() - enemy_->GetHipWorldPosition();
	directionToPlayer.y = 0.0f;
	directionToPlayer = Mathf::Normalize(directionToPlayer);

	//敵をプレイヤーの方向に回転
	enemy_->Rotate(directionToPlayer);
}

void EnemyStateRoot::PerformNextAction()
{
	//次のアクションを選択
	int nextAction = ChooseNextAction();

	//前のアクションを更新
	preAction_ = nextAction;

	//アクションを実行
	ExecuteAction(nextAction);                 
}

int EnemyStateRoot::ChooseNextAction() const
{
	//次のアクション
	int nextAction = -1;

	//次のアクションが前回と異なるかをチェックしながらランダムに選択
	while (nextAction == preAction_)
	{
		nextAction = RandomGenerator::GetRandomInt(0, kMaxCloseRangeActions);
	}

	return nextAction;
}

void EnemyStateRoot::ExecuteAction(int action)
{
	switch (action)
	{
	case kTackle:
		enemy_->ChangeState(new EnemyStateTackle());

		break;
	case kJumpAttack:
		enemy_->ChangeState(new EnemyStateJumpAttack());

		break;
	case kComboAttack:
		enemy_->ChangeState(new EnemyStateComboAttack());

		break;
	}
}