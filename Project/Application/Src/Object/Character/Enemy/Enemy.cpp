#include "Enemy.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Player/Player.h"

void Enemy::Initialize()
{
	//基底クラスの初期化
	BaseCharacter::Initialize();

	//攻撃状態の初期化
	ResetAttackState();

	//状態の初期化
	ChangeState("Idle");

	//デバッグのフラグを立てる
	isDebug_ = true;
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
	//状態の衝突判定処理
	currentState_->OnCollision(gameObject);
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
	attackInterval_ = RandomGenerator::GetRandomFloat(rootParameters_.minActionInterval, rootParameters_.maxActionInterval);
}

void Enemy::InitializeActionMap()
{
	//距離を比較するための共通関数を作成
	auto withinCloseRangeAttackRange = [this]() { return distanceToPlayer_ <= rootParameters_.closeRangeAttackDistance; };
	auto withinApproachRange = [this]() { return distanceToPlayer_ <= rootParameters_.approachDistance; };
	auto withinRangedAttackRange = [this]() { return distanceToPlayer_ <= rootParameters_.rangedAttackDistance; };

	//アクションマップの初期化
	actionMap_ = {
		{"None", [this]() { return true; }},
		{"Idle", [this, withinCloseRangeAttackRange]() { return withinCloseRangeAttackRange(); }},
		{"MoveTowardPlayer", [this, withinApproachRange]() { return !withinApproachRange(); }},
		{"MoveAwayFromPlayer", [this, withinCloseRangeAttackRange]() { return withinCloseRangeAttackRange(); }},
		{"MoveSideToPlayer", [this, withinApproachRange, withinCloseRangeAttackRange]() {return withinApproachRange() && !withinCloseRangeAttackRange(); }},
		{"Dodge", [this, withinRangedAttackRange]() {return withinRangedAttackRange(); }},
		{"Approach", [this, withinApproachRange, withinRangedAttackRange]() {return !withinApproachRange(); }},
		{"Attack", [this, withinCloseRangeAttackRange]() { return withinCloseRangeAttackRange(); }},
		{"VerticalSlash", [this, withinCloseRangeAttackRange]() { return withinCloseRangeAttackRange(); }},
		{"SpinSlash", [this, withinCloseRangeAttackRange]() { return withinCloseRangeAttackRange(); }},
		{"ComboSlash1", [this, withinCloseRangeAttackRange]() { return withinCloseRangeAttackRange(); }},
		{"ComboSlash2", [this, withinApproachRange]() { return withinApproachRange(); }},
	};
}

void Enemy::InitializeAudio()
{
	//基底クラスの呼び出し
	BaseCharacter::InitializeAudio();
}

void Enemy::InitializeAnimator()
{
	//基底クラスの呼び出し
	BaseCharacter::InitializeAnimator();

	//アニメーション名とファイルパス
	std::vector<std::pair<std::string, std::string>> animations = {
		{"Idle", "Enemy/Animations/Idle.gltf"}, {"Walk1", "Enemy/Animations/Walk1.gltf"}, {"Walk2", "Enemy/Animations/Walk2.gltf"}, {"Walk3", "Enemy/Animations/Walk3.gltf"}, {"Walk4", "Enemy/Animations/Walk4.gltf"}, 
		{"Run", "Enemy/Animations/Run.gltf"}, {"Dash", "Enemy/Animations/Dash.gltf"}, {"Dodge", "Enemy/Animations/Dodge.gltf"},{"HitStun", "Enemy/Animations/HitStun.gltf"}, {"Knockdown", "Enemy/Animations/Knockdown.gltf"},
		{"StandUp", "Enemy/Animations/StandUp.gltf"},{"Death", "Enemy/Animations/Death.gltf"}, {"VerticalSlash", "Enemy/Animations/VerticalSlash.gltf"}, {"ComboSlash1", "Enemy/Animations/ComboSlash1.gltf"},
		{"ComboSlash2", "Enemy/Animations/ComboSlash2.gltf"}, {"SpinSlash", "Enemy/Animations/SpinSlash.gltf"},
	};

	//アニメーションを追加
	for (const auto& [name, path] : animations)
	{
		animator_->AddAnimation(name, AnimationManager::Create(path));
	}

	//通常アニメーションを再生
	animator_->PlayAnimation("Idle", 1.0f, true);
}

void Enemy::InitializeUISprites()
{
	//テクスチャの名前を設定
	hpTextureNames_ = { {
		{"barBack_horizontalLeft.png","barBack_horizontalMid.png","barBack_horizontalRight.png"},
		{"barBlue_horizontalLeft.png","barBlue_horizontalBlue.png","barBlue_horizontalRight.png"},
		}
	};

	//スプライトの座標を設定
	hpBarSegmentPositions_ = { {
		{ {	{711.0f, 40.0f}, {720.0f, 40.0f}, {1200.0f, 40.0f},}},
		{ { {711.0f, 40.0f}, {720.0f, 40.0f}, {1200.0f, 40.0f},}},
		}
	};

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
	if (elapsedAttackTime_ > rootParameters_.dodgeStartTime)
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
	globalVariables->AddItem(name_, "DodgeStartTime", rootParameters_.dodgeStartTime);
	globalVariables->AddItem(name_, "MinActionInterval", rootParameters_.minActionInterval);
	globalVariables->AddItem(name_, "MaxActionInterval", rootParameters_.maxActionInterval);
	globalVariables->AddItem(name_, "MinWaitTimeBeforeMovement", rootParameters_.minWaitTimeBeforeMovement);
	globalVariables->AddItem(name_, "MaxWaitTimeBeforeMovement", rootParameters_.maxWaitTimeBeforeMovement);
}

void Enemy::ApplyGlobalVariables()
{
	//基底クラスの呼び出し
	BaseCharacter::ApplyGlobalVariables();
	//環境変数のインスタンスを取得
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	rootParameters_.dodgeStartTime = globalVariables->GetFloatValue(name_, "DodgeStartTime");
	rootParameters_.minActionInterval = globalVariables->GetFloatValue(name_, "MinActionInterval");
	rootParameters_.maxActionInterval = globalVariables->GetFloatValue(name_, "MaxActionInterval");
	rootParameters_.minWaitTimeBeforeMovement = globalVariables->GetFloatValue(name_, "MinWaitTimeBeforeMovement");
	rootParameters_.maxWaitTimeBeforeMovement = globalVariables->GetFloatValue(name_, "MaxWaitTimeBeforeMovement");
}