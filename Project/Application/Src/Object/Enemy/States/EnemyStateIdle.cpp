#include "EnemyStateIdle.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Engine/Utilities/RandomGenerator.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Player/Player.h"
#include "EnemyStateTackle.h"

void EnemyStateIdle::Initialize()
{
	//次の攻撃までの時間を決める
	attackDuration_ = RandomGenerator::GetRandomFloat(minAttackDuration_, maxAttackDuration_);
}

void EnemyStateIdle::Update()
{
	//差分ベクトルを計算
	Player* player = GameObjectManager::GetInstance()->GetGameObject<Player>();
	TransformComponent* playerTransformComponent = player->GetComponent<TransformComponent>();
	TransformComponent* enemyTransformComponent = enemy_->GetComponent<TransformComponent>();
	Vector3 sub = playerTransformComponent->GetWorldPosition() - enemyTransformComponent->GetWorldPosition();
	sub.y = 0.0f;

	//距離を計算
	float distance = Mathf::Length(sub);

	//プレイヤーとの距離が一定以上離れていたら移動させる
	if (distance >= 10.0f)
	{
		enemy_->velocity = Mathf::Normalize(sub) * moveSpeed_;
		enemyTransformComponent->worldTransform_.translation_ += enemy_->velocity * GameTimer::GetDeltaTime();
	}

	//回転処理
	sub = Mathf::Normalize(sub);
	Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, sub));
	float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, sub);
	enemy_->destinationQuaternion_ = Mathf::Normalize(Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot)));

	//攻撃状態に遷移
	attackTimer_ += GameTimer::GetDeltaTime();
	if (attackTimer_ > attackDuration_)
	{
		uint32_t attackNum = RandomGenerator::GetRandomInt(0, 0);
		IEnemyState* newState = nullptr;
		switch (attackNum)
		{
		case 0:
			enemy_->ChangeState(new EnemyStateTackle());
			break;
		}
	}

	//移動限界座標
	const float kMoveLimitX = 76;
	const float kMoveLimitZ = 76;
	enemyTransformComponent->worldTransform_.translation_.x = std::max<float>(enemyTransformComponent->worldTransform_.translation_.x, -kMoveLimitX);
	enemyTransformComponent->worldTransform_.translation_.x = std::min<float>(enemyTransformComponent->worldTransform_.translation_.x, +kMoveLimitX);
	enemyTransformComponent->worldTransform_.translation_.z = std::max<float>(enemyTransformComponent->worldTransform_.translation_.z, -kMoveLimitZ);
	enemyTransformComponent->worldTransform_.translation_.z = std::min<float>(enemyTransformComponent->worldTransform_.translation_.z, +kMoveLimitZ);
}

void EnemyStateIdle::Draw(const Camera& camera)
{

}