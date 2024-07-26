#include "EnemyStateDash.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/RandomGenerator.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRoot.h"
#include "Application/Src/Object/Enemy/States/EnemyStateTackle.h"

void EnemyStateDash::Initialize()
{
	//アニメーションの設定
	ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
	modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.003");
	modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
	modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);
}

void EnemyStateDash::Update()
{
	//デバッグのフラグが立っているとき
	ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
	if (enemy_->isDebug_)
	{
		modelComponent->GetModel()->GetAnimation()->SetAnimationTime(enemy_->animationTime_);
	}

	//アニメーションの再生が終了していたら
	if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
	{
		//アニメーションのループフラグをfalseにする
		modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);

		//アニメーション後の座標を取得
		WorldTransform hipWorldTransform = modelComponent->GetModel()->GetAnimation()->GetJointWorldTransform("mixamorig:Hips");
		Vector3 animatedTransform = {
			hipWorldTransform.matWorld_.m[3][0],
			0.0f,
			hipWorldTransform.matWorld_.m[3][2],
		};

		//アニメーション後の座標を代入
		TransformComponent* transformComponent = enemy_->GetComponent<TransformComponent>();
		transformComponent->worldTransform_.translation_ = animatedTransform;

		//プレイヤーとの距離を計算
		Player* player = GameObjectManager::GetInstance()->GetGameObject<Player>();
		TransformComponent* playerTransformComponent = player->GetComponent<TransformComponent>();
		float distance = Mathf::Length(playerTransformComponent->GetWorldPosition() - animatedTransform);

		//プレイヤーと一定距離にいた場合
		if (distance > enemy_->rootParameters_.closeRangeDistance)
		{
			//攻撃状態に遷移
			enemy_->ChangeState(new EnemyStateTackle());
		}
		else
		{
			//通常状態に遷移
			enemy_->ChangeState(new EnemyStateRoot());
		}
	}
}

void EnemyStateDash::Draw(const Camera& camera)
{
}
