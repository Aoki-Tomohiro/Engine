#include "EnemyStateRoot.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateDash.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRunTowardsPlayer.h"
#include "Application/Src/Object/Enemy/States/EnemyStateTackle.h"
#include "Application/Src/Object/Enemy/States/EnemyStateJumpAttack.h"
#include "Application/Src/Object/Enemy/States/EnemyStateComboAttack.h"
#include "Application/Src/Object/Enemy/States/EnemyStateEarthSpikeAttack.h"
#include "Application/Src/Object/Enemy/States/EnemyStateLaserBeam.h"
#include "Application/Src/Object/Player/Player.h"

void EnemyStateRoot::Initialize()
{
	//アニメーションブレンドを有効にする
	enemy_->SetIsAnimationBlending(true);

	//次の行動の間隔をランダムに設定
	currentActionInterval_ = RandomGenerator::GetRandomFloat(enemy_->GetRootParameters().minActionInterval, enemy_->GetRootParameters().maxActionInterval);

	//プレイヤーと敵の座標を取得
	Vector3 playerPosition = GameObjectManager::GetInstance()->GetMutableGameObject<Player>("")->GetHipWorldPosition();
	Vector3 enemyPosition = enemy_->GetHipWorldPosition();

	//プレイヤーへの方向ベクトルを計算
	Vector3 directionToPlayer = playerPosition - enemyPosition;
	directionToPlayer.y = 0.0f;

	//プレイヤーに近づいていた場合
	if (Mathf::Length(directionToPlayer) < enemy_->GetRootParameters().stopDistance)
	{
		behaviorRequest_ = MovementBehavior::kIdle;
	}
	else if (Mathf::Length(directionToPlayer) > enemy_->GetRootParameters().approachDistance)
	{
		int32_t nextIndex = RandomGenerator::GetRandomInt(0, 1);
		behaviorRequest_ = nextIndex == 0 ? MovementBehavior::kTowardsPlayer : MovementBehavior::kSideways;
	}

	//ゲームが終了していたら通常アニメーションを再生
	if (enemy_->GetIsGameFinished())
	{
		enemy_->PlayAnimation("Idle", 1.0f, true);
	}
}

void EnemyStateRoot::Update()
{
	//ゲームが終了していたら処理を飛ばす
	if (enemy_->GetIsGameFinished() || enemy_->GetIsDebug())
	{
		return;
	}

	//アニメーションブレンドが終了したら元のブレンド時間に戻す
	ResetBlendDurationAfterAnimation(0.2f);

	//敵をプレイヤーの方向に回転させる
	RotateTowardsPlayer();

	//ビヘイビアの初期化
	InitializeMovementBehavior();

	//ビヘイビアの更新
	UpdateMovementBehavior();

	//アクションタイマーの更新
	UpdateActionTimer();
}

void EnemyStateRoot::OnCollision(GameObject* other)
{
	//衝突処理
	enemy_->ProcessCollisionImpact(other, true);
}

void EnemyStateRoot::UpdateActionTimer()
{
	//アクションタイマーの更新
	actionTimer_ += GameTimer::GetDeltaTime() * enemy_->GetTimeScale();

	//プレイヤーと敵の座標を取得
	Vector3 playerPosition = GameObjectManager::GetInstance()->GetMutableGameObject<Player>("")->GetHipWorldPosition();
	Vector3 enemyPosition = enemy_->GetHipWorldPosition();

	//プレイヤーへの方向ベクトルを計算
	Vector3 directionToPlayer = playerPosition - enemyPosition;
	directionToPlayer.y = 0.0f;

	//プレイヤーとの距離を取得
	float distanceToPlayer = Mathf::Length(directionToPlayer);

	//アクションタイマーが経過している場合、またはスタン復帰後かつプレイヤーとの距離が近い場合にアクション実行
	if (actionTimer_ > currentActionInterval_ || (enemy_->GetIsStunRecovered() && distanceToPlayer < enemy_->GetRootParameters().stopDistance))
	{
		if (enemy_->GetIsStunRecovered())
		{
			//スタン復帰状態をリセット
			enemy_->SetIsStunRecovered(false); 
		}
		//距離に応じたアクションを実行
		PerformActionBasedOnDistance();  
	}
}

void EnemyStateRoot::PerformActionBasedOnDistance()
{
	//プレイヤーと敵の座標を取得
	Vector3 playerPosition = GameObjectManager::GetInstance()->GetMutableGameObject<Player>("")->GetHipWorldPosition();
	Vector3 enemyPosition = enemy_->GetHipWorldPosition();

	//プレイヤーへの方向ベクトルを計算
	Vector3 directionToPlayer = playerPosition - enemyPosition;
	directionToPlayer.y = 0.0f;

	//プレイヤーとの距離を取得
	float distanceToPlayer = Mathf::Length(directionToPlayer);
	float closeRangeDistance = enemy_->GetRootParameters().closeRangeDistance;
	float stopDistance = enemy_->GetRootParameters().stopDistance;

	//プレイヤーとの距離が近距離攻撃の距離より離れていた場合
	if (distanceToPlayer > enemy_->GetRootParameters().closeRangeDistance)
	{
		//攻撃タイプをランダムに決定
		Enemy::AttackType attackType = static_cast<Enemy::AttackType>(RandomGenerator::GetRandomInt(0, static_cast<int>(Enemy::AttackType::kMaxAttackTypes)));

		switch (attackType)
		{
		case Enemy::AttackType::kCloseRange:
			HandleCloseRangeAttack(distanceToPlayer, stopDistance);
			break;
		case Enemy::AttackType::kRanged:
			HandleRangedAttack();
			break;
		}
	}
	//プレイヤーが近距離攻撃の範囲にいたら近距離攻撃をさせる
	else
	{
		Enemy::CloseRangeAttack nextAction = Enemy::CloseRangeAttack::kMaxCloseRangeAttacks;
		do
		{
			nextAction = static_cast<Enemy::CloseRangeAttack>(RandomGenerator::GetRandomInt(0, static_cast<int>(Enemy::CloseRangeAttack::kMaxCloseRangeAttacks)));
		} while (nextAction == enemy_->GetPreviousCloseRangeAttack());
		PerformCloseRangeAttack(static_cast<int>(nextAction));
	}
}

void EnemyStateRoot::HandleCloseRangeAttack(float distanceToPlayer, float stopDistance)
{
	if (distanceToPlayer > stopDistance)
	{
		Enemy::ApproachAction nextAction = static_cast<Enemy::ApproachAction>(RandomGenerator::GetRandomInt(0, static_cast<int>(Enemy::ApproachAction::kMaxApproachActions)));
		PerformApproachAction(static_cast<int>(nextAction));
	}
	else
	{
		Enemy::CloseRangeAttack nextAction = Enemy::CloseRangeAttack::kMaxCloseRangeAttacks;
		do
		{
			nextAction = static_cast<Enemy::CloseRangeAttack>(RandomGenerator::GetRandomInt(0, static_cast<int>(Enemy::CloseRangeAttack::kMaxCloseRangeAttacks)));
		} while (nextAction == enemy_->GetPreviousCloseRangeAttack());
		PerformCloseRangeAttack(static_cast<int>(nextAction));
	}
}

void EnemyStateRoot::HandleRangedAttack()
{
	Enemy::RangedAttack nextAction = Enemy::RangedAttack::kMaxRangedAttacks;
	do
	{
		nextAction = static_cast<Enemy::RangedAttack>(RandomGenerator::GetRandomInt(0, static_cast<int>(Enemy::RangedAttack::kMaxRangedAttacks)));
	} while (nextAction == enemy_->GetPreviousRangedAttack());
	PerformRangedAttack(static_cast<int>(nextAction));
}

void EnemyStateRoot::PerformApproachAction(const int action)
{
	switch (static_cast<Enemy::ApproachAction>(action))
	{
	case Enemy::ApproachAction::kDashForward:
		//ダッシュ前進の処理
		enemy_->ChangeState(new EnemyStateDash());
		break;
	case Enemy::ApproachAction::kRunTowardsPlayer:
		//接近の処理
		enemy_->ChangeState(new EnemyStateRunTowardsPlayer());
		break;
	}
}

void EnemyStateRoot::PerformCloseRangeAttack(const int attack)
{
	//Enemy::CloseRangeAttackにキャスト
	Enemy::CloseRangeAttack nextAttack = static_cast<Enemy::CloseRangeAttack>(attack);
	//現在の攻撃を前回の攻撃として設定
	enemy_->SetPreviousCloseRangeAttack(nextAttack);

	//攻撃タイプに応じた処理を実行
	switch (nextAttack)
	{
	case Enemy::CloseRangeAttack::kTackle:
		//タックル攻撃の処理
		enemy_->ChangeState(new EnemyStateTackle());
		break;
	case Enemy::CloseRangeAttack::kJumpAttack:
		//ジャンプ攻撃の処理
		enemy_->ChangeState(new EnemyStateJumpAttack());
		break;
	case Enemy::CloseRangeAttack::kComboAttack:
		//コンボ攻撃の処理
		enemy_->ChangeState(new EnemyStateComboAttack());
		break;
	}
}

void EnemyStateRoot::PerformRangedAttack(const int attack)
{
	//Enemy::RangedAttackにキャスト
	Enemy::RangedAttack nextAttack = static_cast<Enemy::RangedAttack>(attack);
	//現在の攻撃を前回の攻撃として設定
	enemy_->SetPreviousRangedAttack(nextAttack);

	//攻撃タイプに応じた処理を実行
	switch (nextAttack)
	{
	case Enemy::RangedAttack::kEarthSpike:
		//地面から突起が出す攻撃の処理
		enemy_->ChangeState(new EnemyStateEarthSpikeAttack());
		break;
	case Enemy::RangedAttack::kLaserBeam:
		//レーザービーム攻撃の処理
		enemy_->ChangeState(new EnemyStateLaserBeam());
		break;
	}
}

void EnemyStateRoot::ResetBlendDurationAfterAnimation(float defaultBlendDuration)
{
	//アニメーションブレンドが終了したら元のブレンド時間に戻す
	if (enemy_->GetIsBlendingCompleted())
	{
		enemy_->SetBlendDuration(defaultBlendDuration);
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

void EnemyStateRoot::InitializeMovementBehavior()
{
	//Behaviorの遷移処理
	if (behaviorRequest_)
	{
		//振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		//各振る舞いごとの初期化を実行
		switch (behavior_)
		{
		case MovementBehavior::kIdle:
			InitializeIdle();
			break;
		case MovementBehavior::kTowardsPlayer:
			InitializeTowardsPlayer();
			break;
		case MovementBehavior::kRetreat:
			InitializeRetreat();
			break;
		case MovementBehavior::kSideways:
			InitializeSideways();
			break;
		}
		behaviorRequest_ = std::nullopt;
	}
}

void EnemyStateRoot::UpdateMovementBehavior()
{
	//Behaviorの実行
	switch (behavior_)
	{
	case MovementBehavior::kIdle:
		UpdateIdle();
		break;
	case MovementBehavior::kTowardsPlayer:
		UpdateTowardsPlayer();
		break;
	case MovementBehavior::kRetreat:
		UpdateRetreat();
		break;
	case MovementBehavior::kSideways:
		UpdateSideways();
		break;
	}
}

void EnemyStateRoot::InitializeIdle()
{
	//タイマーのリセットと待機時間の設定
	ResetWaitTimeBeforeMovement();

	//アニメーションの再生
	enemy_->PlayAnimation("Idle", 1.0f, true);

	//次の振る舞いを設定
	int32_t randomIndex = RandomGenerator::GetRandomInt(0, 1);
	nextBehavior_ = (randomIndex == 0) ? MovementBehavior::kRetreat : MovementBehavior::kSideways;
}

void EnemyStateRoot::UpdateIdle()
{
	//待機時間タイマーを進めて状態を更新する
	UpdateWaitTimeAndRequestMovement(nextBehavior_);
}

void EnemyStateRoot::InitializeTowardsPlayer()
{
	//タイマーのリセットと待機時間の設定
	ResetWaitTimeBeforeMovement();

	//アニメーションの再生
	enemy_->PlayAnimation("Walk1", 1.0f, true);
}

void EnemyStateRoot::UpdateTowardsPlayer()
{
	//プレイヤーと敵の座標を取得
	Vector3 playerPosition = GameObjectManager::GetInstance()->GetMutableGameObject<Player>("")->GetHipWorldPosition();
	Vector3 enemyPosition = enemy_->GetHipWorldPosition();

	//プレイヤーへの方向ベクトルを計算
	Vector3 directionToPlayer = playerPosition - enemyPosition;
	directionToPlayer.y = 0.0f;

	//速度を計算して移動させる
	velocity_ = Mathf::Normalize(directionToPlayer) * enemy_->GetRootParameters().moveSpeed;
	enemy_->Move(velocity_);

	//プレイヤーに近づいていた場合
	if (Mathf::Length(directionToPlayer) < enemy_->GetRootParameters().stopDistance)
	{
		behaviorRequest_ = MovementBehavior::kIdle;
	}
	else if (Mathf::Length(directionToPlayer) < enemy_->GetRootParameters().approachDistance)
	{
		UpdateWaitTimeAndRequestMovement(MovementBehavior::kSideways);
	}
}

void EnemyStateRoot::InitializeRetreat()
{
	//タイマーのリセットと待機時間の設定
	ResetWaitTimeBeforeMovement();

	//アニメーションの再生
	enemy_->PlayAnimation("Walk2", 1.0f, true);

	//次の振る舞いを設定
	int32_t randomIndex = RandomGenerator::GetRandomInt(0, 1);
	nextBehavior_ = (randomIndex == 0) ? MovementBehavior::kTowardsPlayer : MovementBehavior::kSideways;
}

void EnemyStateRoot::UpdateRetreat()
{
	//プレイヤーと敵の座標を取得
	Vector3 playerPosition = GameObjectManager::GetInstance()->GetMutableGameObject<Player>("")->GetHipWorldPosition();
	Vector3 enemyPosition = enemy_->GetHipWorldPosition();

	//プレイヤーへの方向ベクトルを計算
	Vector3 directionToPlayer = playerPosition - enemyPosition;
	directionToPlayer.y = 0.0f;

	//速度を計算して移動させる
	velocity_ = Mathf::Normalize(directionToPlayer) * enemy_->GetRootParameters().moveSpeed * -1.0f;
	enemy_->Move(velocity_);

	//プレイヤーに離れていた場合
	if (Mathf::Length(directionToPlayer) > enemy_->GetRootParameters().stopDistance)
	{
		//待機時間タイマーを進めて状態を更新する
		UpdateWaitTimeAndRequestMovement(nextBehavior_);
	}
}

void EnemyStateRoot::InitializeSideways()
{
	//タイマーのリセットと待機時間の設定
	ResetWaitTimeBeforeMovement();

	//移動方向をランダムで決める
	int32_t randomIndex = RandomGenerator::GetRandomInt(0, 1);
	horizontalMovementDirection_ = (randomIndex == 0) ? -1.0f : 1.0f;

	//アニメーションの再生
	std::string animationName = (randomIndex == 0) ? "Walk3" : "Walk4";
	enemy_->PlayAnimation(animationName, 1.0f, true);

	//次の振る舞いを設定
	randomIndex = RandomGenerator::GetRandomInt(0, 1);
	nextBehavior_ = (randomIndex == 0) ? MovementBehavior::kTowardsPlayer : MovementBehavior::kSideways;
}

void EnemyStateRoot::UpdateSideways()
{
	//速度を計算して移動させる
	velocity_ = Vector3{ 1.0f,0.0f,0.0f } * enemy_->GetRootParameters().moveSpeed * horizontalMovementDirection_;
	velocity_ = Mathf::RotateVector(velocity_, enemy_->GetDestinationQuaternion());
	enemy_->Move(velocity_);

	//待機時間タイマーを進めて状態を更新する
	UpdateWaitTimeAndRequestMovement(nextBehavior_);
}

void EnemyStateRoot::ResetWaitTimeBeforeMovement()
{
	//待機時間のタイマーをリセット
	waitTimeBeforeMovementTimer_ = 0.0f;

	//ランダムな待機時間を設定
	currentWaitTimeBeforeMovement_ = RandomGenerator::GetRandomFloat(
		enemy_->GetRootParameters().minWaitTimeBeforeMovement,
		enemy_->GetRootParameters().maxWaitTimeBeforeMovement
	);
}

void EnemyStateRoot::UpdateWaitTimeAndRequestMovement(const MovementBehavior& behavior)
{
	//待機時間のタイマーを更新
	waitTimeBeforeMovementTimer_ += GameTimer::GetDeltaTime();

	//待機時間が経過した場合、指定された行動をリクエスト
	if (waitTimeBeforeMovementTimer_ > currentWaitTimeBeforeMovement_)
	{
		behaviorRequest_ = behavior;
	}
}