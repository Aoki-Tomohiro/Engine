#include "EnemyStateRoot.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/RandomGenerator.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateTackle.h"
#include "Application/Src/Object/Enemy/States/EnemyStateMissile.h"
#include "Application/Src/Object/Player/Player.h"

void EnemyStateRoot::Initialize()
{
	//次の行動の間隔を決める
	currentActionInterval_ = RandomGenerator::GetRandomFloat(enemy_->rootParameters_.minActionInterval_, enemy_->rootParameters_.maxActionInterval_);
}

void EnemyStateRoot::Update()
{
	//デバッグのフラグが立っているとき
	ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
	if (enemy_->isDebug_)
	{
		modelComponent->GetModel()->GetAnimation()->SetAnimationTime(enemy_->animationTime_);
		return;
	}

	//プレイヤーのトランスフォームを取得
	TransformComponent* playerTransformComponent = GameObjectManager::GetInstance()->GetGameObject<Player>()->GetComponent<TransformComponent>();

	//差分ベクトルを計算
	TransformComponent* enemyTransformComponent = enemy_->GetComponent<TransformComponent>();
	Vector3 sub = playerTransformComponent->GetWorldPosition() - enemyTransformComponent->GetWorldPosition();
	sub.y = 0.0f;

	//距離を計算
	float distance = Mathf::Length(sub);

	//プレイヤーとの距離が一定以上離れていたら移動させる
	if (distance >= enemy_->rootParameters_.stopDistance)
	{
		enemy_->velocity = Mathf::Normalize(sub) * enemy_->rootParameters_.moveSpeed;
		enemyTransformComponent->worldTransform_.translation_ += enemy_->velocity * GameTimer::GetDeltaTime() * enemy_->timeScale_;
	}

	//回転処理
	sub = Mathf::Normalize(sub);
	Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, sub));
	float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, sub);
	enemy_->destinationQuaternion_ = Mathf::Normalize(Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot)));

	//アクションタイマーを進める
	actionTimer_ += GameTimer::GetDeltaTime() * enemy_->timeScale_;

	//アクションタイマーが一定間隔を超えていたら
	if (actionTimer_ > currentActionInterval_)
	{
		//プレイヤーとの距離が近距離行動の距離より近かった場合
		if (distance < enemy_->rootParameters_.closeRangeDistance)
		{
			int nextAction = RandomGenerator::GetRandomInt(0, kMaxCloseRangeActions);
			switch (nextAction)
			{
			case kTackle:
				enemy_->ChangeState(new EnemyStateTackle());
				break;
			}
		}
		else
		{
			int nextAction = RandomGenerator::GetRandomInt(0, kMaxCloseRangeActions);
			switch (nextAction)
			{
			case kDash:
				break;
			case kMagicAttack:
				enemy_->ChangeState(new EnemyStateMissile());
				break;
			}
		}
	}
}

void EnemyStateRoot::Draw(const Camera& camera)
{
}