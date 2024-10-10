#include "Enemy.h"
#include "Application/Src/Object/Character/States/EnemyStates/EnemyStateRoot.h"
#include "Application/Src/Object/Character/States/EnemyStates/EnemyStateStun.h"
#include "Application/Src/Object/Character/States/EnemyStates/EnemyStateDeath.h"

void Enemy::Initialize()
{
	//基底クラスの初期化
	ICharacter::Initialize();

	//HPの初期化
	maxHp_ = 800.0f;
	hp_ = maxHp_;

	//状態の初期化
	ChangeState(new EnemyStateRoot());
}

void Enemy::Update()
{
	//モデルシェイクでずれた分の座標をリセット
	ResetToOriginalPosition();

	//モデルシェイクの更新
	UpdateModelShake();

	//タイトルシーンにいない場合
	if (!isInTitleScene_)
	{
		//状態の更新
		state_->Update();
	}

	//モデルシェイクを適用
	ApplyModelShake();

	//死亡状態かどうかを確認する
	CheckAndTransitionToDeath();

	//基底クラスの更新
	ICharacter::Update();
}

void Enemy::OnCollision(GameObject* gameObject)
{
	//状態の衝突判定処理
	state_->OnCollision(gameObject);
}

void Enemy::ChangeState(IEnemyState* newState)
{
	//新しい状態の設定
	newState->SetEnemy(this);

	//新しい状態の初期化
	newState->Initialize();

	//現在の状態を新しい状態に更新
	state_.reset(newState);
}

void Enemy::StartModelShake()
{
	modelShake_.isActive = true;
	modelShake_.elapsedTime = 0.0f;
	modelShake_.originalPosition = GetPosition();
}

void Enemy::UpdateModelShake()
{
	//モデルシェイクの経過時間を進める
	modelShake_.elapsedTime += GameTimer::GetDeltaTime();

	//モデルシェイクの経過時間が一定値を超えていたら終了させる
	if (modelShake_.elapsedTime > modelShake_.duration)
	{
		modelShake_.isActive = false;
	}
}

void Enemy::ApplyModelShake()
{
	//モデルシェイクが有効な場合
	if (modelShake_.isActive)
	{
		//モデルシェイクの強度をランダムで決める
		Vector3 intensity = {
			RandomGenerator::GetRandomFloat(-modelShake_.intensity.x,modelShake_.intensity.x),
			RandomGenerator::GetRandomFloat(-modelShake_.intensity.y,modelShake_.intensity.y),
			RandomGenerator::GetRandomFloat(-modelShake_.intensity.z,modelShake_.intensity.z),
		};

		//座標をずらす
		Vector3 currentPosition = GetPosition();
		modelShake_.originalPosition = currentPosition;
		currentPosition += intensity * GameTimer::GetDeltaTime();
		SetPosition(currentPosition);
	}
}

void Enemy::ResetToOriginalPosition()
{
	if (modelShake_.isActive)
	{
		SetPosition(modelShake_.originalPosition);
	}
}

void Enemy::CheckAndTransitionToDeath()
{
	//敵の体力が0を下回っていた場合
	if (hp_ <= 0.0f)
	{
		//死亡状態に遷移
		if (!isDead_)
		{
			ChangeState(new EnemyStateDeath());
		}

		//死亡フラグを立てる
		isDead_ = true;
	}
}

void Enemy::InitializeAnimator()
{
	//基底クラスの呼び出し
	ICharacter::InitializeAnimator();

	//アニメーション名とファイルパス
	std::vector<std::pair<std::string, std::string>> animations = {
		{"Idle", "Enemy/Animations/Idle.gltf"}, {"Walk1", "Enemy/Animations/Walk1.gltf"}, {"Walk2", "Enemy/Animations/Walk2.gltf"}, {"Walk3", "Enemy/Animations/Walk3.gltf"},
		{"Walk4", "Enemy/Animations/Walk4.gltf"}, {"Run", "Enemy/Animations/Run.gltf"}, {"Dash", "Enemy/Animations/Dash.gltf"}, {"TackleAttack", "Enemy/Animations/Tackle.gltf"},
		{"JumpAttack", "Enemy/Animations/JumpAttack.gltf"}, {"ComboAttack", "Enemy/Animations/ComboAttack.gltf"}, {"EarthSpike", "Enemy/Animations/EarthSpike.gltf"},
		{"LaserBeam", "Enemy/Animations/LaserBeam.gltf"}, {"HitStun", "Enemy/Animations/HitStun.gltf"}, {"Knockdown", "Enemy/Animations/Knockdown.gltf"},
		{"StandUp", "Enemy/Animations/StandUp.gltf"}, {"Death", "Enemy/Animations/Death.gltf"}
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
	ICharacter::InitializeUISprites();
}

void Enemy::TransitionToStunState()
{
	//スタン状態に遷移
	ChangeState(new EnemyStateStun());
}