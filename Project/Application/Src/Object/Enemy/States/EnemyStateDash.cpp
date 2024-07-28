#include "EnemyStateDash.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Engine/Utilities/RandomGenerator.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRoot.h"
#include "Application/Src/Object/Enemy/States/EnemyStateTackle.h"

void EnemyStateDash::Initialize()
{
	//アニメーションの設定
	ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
	modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.005");
	modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
	modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);

	//現在のColliderの中心座標を保存
	AABBCollider* collider = enemy_->GetComponent<AABBCollider>();
	currentCenter_ = collider->GetCenter();
	//腰のJointとコライダーの中心のオフセット値を保存
	offset_ = currentCenter_ - enemy_->GetHipLocalPosition();
}

void EnemyStateDash::Update()
{
	//デバッグのフラグが立っているとき
	ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
	if (enemy_->isDebug_)
	{
		modelComponent->GetModel()->GetAnimation()->SetAnimationTime(enemy_->animationTime_);
	}

	//Transformを取得
	TransformComponent* transformComponent = enemy_->GetComponent<TransformComponent>();
	//移動させる
	enemy_->velocity = Mathf::RotateVector({ 0.0f,0.0f,enemy_->dashParameters_.dashSpeed }, enemy_->destinationQuaternion_);
	transformComponent->worldTransform_.translation_ += enemy_->velocity * GameTimer::GetDeltaTime() * enemy_->timeScale_;

	//Colliderを取得
	AABBCollider* collider = enemy_->GetComponent<AABBCollider>();
	//ローカルの腰の座標を取得
	Vector3 hipLocalPosition = enemy_->GetHipLocalPosition();
	//敵のワールド座標からずれた分だけ中心座標をずらす
	collider->SetCenter(hipLocalPosition + offset_);

	//プレイヤーとの距離を計算
	Player* player = GameObjectManager::GetInstance()->GetGameObject<Player>();
	TransformComponent* playerTransformComponent = player->GetComponent<TransformComponent>();
	Vector3 postAnimationPosition = enemy_->GetHipWorldPosition();
	float distance = Mathf::Length(playerTransformComponent->GetWorldPosition() - postAnimationPosition);

	//プレイヤーとの距離が一定値より近かったら
	if (distance < enemy_->dashParameters_.attackDistance)
	{
		//アニメーションのループフラグをfalseにする
		modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);

		//アニメーション後の座標を代入
		transformComponent->worldTransform_.translation_ = { postAnimationPosition.x, 0.0f, postAnimationPosition.z };

		//Colliderの中心座標を元に戻す
		collider->SetCenter(currentCenter_);

		//プレイヤーの方向に回転させる
		Vector3 direction = Mathf::Normalize(playerTransformComponent->GetWorldPosition() - postAnimationPosition);
		direction.y = 0.0f;
		Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, direction));
		float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, direction);
		enemy_->destinationQuaternion_ = Mathf::Normalize(Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot)));

		//攻撃状態に遷移
		enemy_->ChangeState(new EnemyStateTackle());
	}
	//アニメーションの再生が終了していたら
	else if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
	{
		//アニメーションのループフラグをfalseにする
		modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);

		//アニメーション後の座標を代入
		transformComponent->worldTransform_.translation_ = { postAnimationPosition.x, 0.0f, postAnimationPosition.z };

		//Colliderの中心座標を元に戻す
		collider->SetCenter(currentCenter_);

		//通常状態に遷移
		enemy_->ChangeState(new EnemyStateRoot());
	}
}

void EnemyStateDash::Draw(const Camera& camera)
{
}
