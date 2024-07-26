#include "EnemyStateJumpAttack.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRoot.h"

void EnemyStateJumpAttack::Initialize()
{
	//アニメーションの設定
	ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
	modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.004");
	modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
	modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);

	//現在のColliderの中心座標を保存
	AABBCollider* collider = enemy_->GetComponent<AABBCollider>();
	currentCenter_ = collider->GetCenter();
	//アニメーション後の座標を取得
	TransformComponent* transformComponent = enemy_->GetComponent<TransformComponent>();
	WorldTransform hipWorldTransform = modelComponent->GetModel()->GetAnimation()->GetJointWorldTransform("mixamorig:Hips");
	Vector3 animatedPosition = {
		hipWorldTransform.matWorld_.m[3][0],
		hipWorldTransform.matWorld_.m[3][1],
		hipWorldTransform.matWorld_.m[3][2],
	};
	//ローカルの腰の座標を取得
	Vector3 hipLocalPosition = animatedPosition - transformComponent->GetWorldPosition();
	offset_ = currentCenter_ - hipLocalPosition;
}

void EnemyStateJumpAttack::Update()
{
	//デバッグのフラグが立っているとき
	ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
	if (enemy_->isDebug_)
	{
		modelComponent->GetModel()->GetAnimation()->SetAnimationTime(enemy_->animationTime_);
	}

	//Transformを取得
	TransformComponent* transformComponent = enemy_->GetComponent<TransformComponent>();
	//Colliderを取得
	AABBCollider* collider = enemy_->GetComponent<AABBCollider>();
	//アニメーション後の座標を取得
	WorldTransform hipWorldTransform = modelComponent->GetModel()->GetAnimation()->GetJointWorldTransform("mixamorig:Hips");
	Vector3 animatedPosition = {
		hipWorldTransform.matWorld_.m[3][0],
		hipWorldTransform.matWorld_.m[3][1],
		hipWorldTransform.matWorld_.m[3][2],
	};
	//ローカルの腰の座標を取得
	Vector3 hipLocalPosition = animatedPosition - transformComponent->GetWorldPosition();
	//敵のワールド座標からずれた分だけ中心座標をずらす
	collider->SetCenter(hipLocalPosition + offset_);


	//アニメーションの再生が終了していたら
	if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
	{
		//アニメーションのループフラグをfalseにする
		modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);

		//アニメーション後の座標を代入
		transformComponent->worldTransform_.translation_ = { animatedPosition.x, 0.0f, animatedPosition.z };

		//Colliderの中心座標を元に戻す
		collider->SetCenter(currentCenter_);

		//通常状態に遷移
		enemy_->ChangeState(new EnemyStateRoot());
	}
}

void EnemyStateJumpAttack::Draw(const Camera& camera)
{

}