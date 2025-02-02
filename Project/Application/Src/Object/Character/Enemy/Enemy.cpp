/**
 * @file Enemy.cpp
 * @brief 敵を管理するファイル
 * @author 青木智滉
 * @date
 */

#include "Enemy.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/BreakableObject/BreakableObject.h"

//敵のレベル
Enemy::Level Enemy::currentLevel_ = Level::Easy;

void Enemy::Initialize()
{
	//基底クラスの初期化
	BaseCharacter::Initialize();

	//攻撃状態の初期化
	ResetAttackState();

	//状態の初期化
	ChangeState("Idle");
}

void Enemy::Update()
{
	//タイトルシーンにいる場合は基底クラスの更新だけする
	if (isInTitleScene_)
	{
		GameObject::Update();
		return;
	}

	//攻撃タイマーの更新
	UpdateAttackTimer();

	//プレイヤーとの距離を更新
	distanceToPlayer_ = CalculateDistanceToPlayer();

	//基底クラスの更新
	BaseCharacter::Update();
}

void Enemy::OnCollision(GameObject* gameObject)
{
	//衝突相手が破壊可能オブジェクトの場合
	if (dynamic_cast<BreakableObject*>(gameObject))
	{
		//押し戻し処理
		ResolveCollision(transform_, gameObject->GetComponent<TransformComponent>(), collider_, gameObject->GetComponent<AABBCollider>());
	}
	else
	{
		//状態の衝突判定処理
		currentState_->OnCollision(gameObject);
	}
}

void Enemy::LookAtPlayer()
{
	//敵の腰のジョイントのワールド座標を取得
	Vector3 enemyPosition = GetJointWorldPosition("mixamorig:Hips");

	//プレイヤーの座標を取得
	Vector3 playerPosition = gameObjectManager_->GetGameObject<Player>("Player")->GetJointWorldPosition("mixamorig:Hips");

	//敵の方向へのベクトルを計算
	Vector3 rotateVector = Mathf::Normalize(playerPosition - enemyPosition);

	//Y軸にだけ回転させたいのでY成分を0にする
	rotateVector.y = 0.0f;

	//回転処理
	Rotate(Mathf::Normalize(rotateVector));
}

void Enemy::ResetAttackState()
{
	//フラグのリセット
	actionFlags_[ActionFlag::kCanAttack] = false;
	actionFlags_[ActionFlag::kCanDodge] = false;

	//攻撃タイマーをリセットし、攻撃間隔をランダムで決める
	elapsedAttackTime_ = 0.0f;
	attackInterval_ = RandomGenerator::GetRandomFloat(rootParameters_[currentLevel_].minActionInterval, rootParameters_[currentLevel_].maxActionInterval);
}

void Enemy::InitializeActionMap()
{
	//距離を比較するための共通関数を作成
	auto withinCloseRangeAttackRange = [this]() { return distanceToPlayer_ <= rootParameters_[currentLevel_].closeRangeAttackDistance; };
	auto withinApproachRange = [this]() { return distanceToPlayer_ <= rootParameters_[currentLevel_].approachDistance; };
	auto withinRangedAttackRange = [this]() { return distanceToPlayer_ <= rootParameters_[currentLevel_].rangedAttackDistance; };

	//アクションマップの初期化
	actionMap_ = {
		{"None", ActionCondition{[this]() {return true; }, [this]() { return true; }}},
		{"Idle", ActionCondition{[this]() {return true; }, [this, withinCloseRangeAttackRange]() { return withinCloseRangeAttackRange(); }}},
		{"MoveTowardPlayer", ActionCondition{[this]() {return true; }, [this, withinApproachRange]() { return !withinApproachRange(); }}},
		{"MoveAwayFromPlayer", ActionCondition{[this]() {return true; }, [this, withinCloseRangeAttackRange]() { return withinCloseRangeAttackRange(); }}},
		{"MoveSideToPlayer", ActionCondition{[this]() {return true; }, [this, withinApproachRange, withinCloseRangeAttackRange]() {return withinApproachRange() && !withinCloseRangeAttackRange(); }}},
		{"Dodge", ActionCondition{[this]() {return true; }, [this, withinRangedAttackRange]() {return withinRangedAttackRange(); }}},
		{"Approach", ActionCondition{[this]() {return true; }, [this, withinCloseRangeAttackRange]() {return !withinCloseRangeAttackRange(); }}},
		{"Attack", ActionCondition{[this]() {return true; }, [this, withinCloseRangeAttackRange]() { return withinCloseRangeAttackRange(); }}},
		{"VerticalSlash", ActionCondition{[this]() {return true; }, [this, withinCloseRangeAttackRange]() { return withinCloseRangeAttackRange(); }}},
		{"SpinSlash", ActionCondition{[this]() {return true; }, [this, withinCloseRangeAttackRange]() { return withinCloseRangeAttackRange(); }}},
		{"ComboSlash1", ActionCondition{[this]() {return true; }, [this, withinCloseRangeAttackRange]() { return withinCloseRangeAttackRange(); }}},
		{"ComboSlash2", ActionCondition{[this]() {return true; }, [this, withinApproachRange]() { return withinApproachRange(); }}},
	};
}

void Enemy::InitializeUISprites()
{
	//基底クラスの呼び出し
	BaseCharacter::InitializeUISprites();
}


float Enemy::CalculateDistanceToPlayer() const
{
	//プレイヤーの座標を取得
	Vector3 playerPosition = gameObjectManager_->GetGameObject<Player>("Player")->GetJointWorldPosition("mixamorig:Hips");

	//敵の座標を取得
	Vector3 enemyPosition = GetJointWorldPosition("mixamorig:Hips");

	//差分ベクトルを計算
	Vector3 diff = playerPosition - enemyPosition;

	//水平方向の距離が知りたいのでY成分を0にする
	diff.y = 0.0f;

	//距離を計算して返す
	return Mathf::Length(diff);
}

void Enemy::UpdateAttackTimer()
{
	//攻撃タイマーの経過時間を進める
	elapsedAttackTime_ += GameTimer::GetDeltaTime() * timeScale_;

	//回避可能な時間に達した場合、回避フラグを立てる
	if (elapsedAttackTime_ > rootParameters_[currentLevel_].dodgeStartTime)
	{
		actionFlags_[ActionFlag::kCanDodge] = true;
	}

	//攻撃可能な時間に達した場合、攻撃フラグを立てる
	if (elapsedAttackTime_ > attackInterval_)
	{
		actionFlags_[ActionFlag::kCanAttack] = true;
	}
}

void Enemy::InitializeGlobalVariables()
{
	//基底クラスの呼び出し
	BaseCharacter::InitializeGlobalVariables();
	//環境変数のインスタンスを取得
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	globalVariables->CreateGroup(name_);
	globalVariables->AddItem(name_, "EasyDodgeStartTime", rootParameters_[Easy].dodgeStartTime);
	globalVariables->AddItem(name_, "EasyMinActionInterval", rootParameters_[Easy].minActionInterval);
	globalVariables->AddItem(name_, "EasyMaxActionInterval", rootParameters_[Easy].maxActionInterval);
	globalVariables->AddItem(name_, "EasyMinWaitTimeBeforeMovement", rootParameters_[Easy].minWaitTimeBeforeMovement);
	globalVariables->AddItem(name_, "EasyMaxWaitTimeBeforeMovement", rootParameters_[Easy].maxWaitTimeBeforeMovement);
	globalVariables->AddItem(name_, "NormalDodgeStartTime", rootParameters_[Normal].dodgeStartTime);
	globalVariables->AddItem(name_, "NormalMinActionInterval", rootParameters_[Normal].minActionInterval);
	globalVariables->AddItem(name_, "NormalMaxActionInterval", rootParameters_[Normal].maxActionInterval);
	globalVariables->AddItem(name_, "NormalMinWaitTimeBeforeMovement", rootParameters_[Normal].minWaitTimeBeforeMovement);
	globalVariables->AddItem(name_, "NormalMaxWaitTimeBeforeMovement", rootParameters_[Normal].maxWaitTimeBeforeMovement);
	globalVariables->AddItem(name_, "HardDodgeStartTime", rootParameters_[Hard].dodgeStartTime);
	globalVariables->AddItem(name_, "HardMinActionInterval", rootParameters_[Hard].minActionInterval);
	globalVariables->AddItem(name_, "HardMaxActionInterval", rootParameters_[Hard].maxActionInterval);
	globalVariables->AddItem(name_, "HardMinWaitTimeBeforeMovement", rootParameters_[Hard].minWaitTimeBeforeMovement);
	globalVariables->AddItem(name_, "HardMaxWaitTimeBeforeMovement", rootParameters_[Hard].maxWaitTimeBeforeMovement);
}

void Enemy::ApplyGlobalVariables()
{
	//基底クラスの呼び出し
	BaseCharacter::ApplyGlobalVariables();
	//環境変数のインスタンスを取得
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	rootParameters_[Easy].dodgeStartTime = globalVariables->GetFloatValue(name_, "EasyDodgeStartTime");
	rootParameters_[Easy].minActionInterval = globalVariables->GetFloatValue(name_, "EasyMinActionInterval");
	rootParameters_[Easy].maxActionInterval = globalVariables->GetFloatValue(name_, "EasyMaxActionInterval");
	rootParameters_[Easy].minWaitTimeBeforeMovement = globalVariables->GetFloatValue(name_, "EasyMinWaitTimeBeforeMovement");
	rootParameters_[Easy].maxWaitTimeBeforeMovement = globalVariables->GetFloatValue(name_, "EasyMaxWaitTimeBeforeMovement");
	rootParameters_[Normal].dodgeStartTime = globalVariables->GetFloatValue(name_, "NormalDodgeStartTime");
	rootParameters_[Normal].minActionInterval = globalVariables->GetFloatValue(name_, "NormalMinActionInterval");
	rootParameters_[Normal].maxActionInterval = globalVariables->GetFloatValue(name_, "NormalMaxActionInterval");
	rootParameters_[Normal].minWaitTimeBeforeMovement = globalVariables->GetFloatValue(name_, "NormalMinWaitTimeBeforeMovement");
	rootParameters_[Normal].maxWaitTimeBeforeMovement = globalVariables->GetFloatValue(name_, "NormalMaxWaitTimeBeforeMovement");
	rootParameters_[Hard].dodgeStartTime = globalVariables->GetFloatValue(name_, "HardDodgeStartTime");
	rootParameters_[Hard].minActionInterval = globalVariables->GetFloatValue(name_, "HardMinActionInterval");
	rootParameters_[Hard].maxActionInterval = globalVariables->GetFloatValue(name_, "HardMaxActionInterval");
	rootParameters_[Hard].minWaitTimeBeforeMovement = globalVariables->GetFloatValue(name_, "HardMinWaitTimeBeforeMovement");
	rootParameters_[Hard].maxWaitTimeBeforeMovement = globalVariables->GetFloatValue(name_, "HardMaxWaitTimeBeforeMovement");
}