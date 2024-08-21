#include "Enemy.h"
#include "Engine/Components/Component/ModelComponent.h"

void Enemy::Initialize()
{
}

void Enemy::Update()
{
}

void Enemy::Draw(const Camera& camera)
{
}

void Enemy::DrawUI()
{
}

void Enemy::OnCollision(GameObject* gameObject)
{
}

void Enemy::OnCollisionEnter(GameObject* gameObject)
{
}

void Enemy::OnCollisionExit(GameObject* gameObject)
{
}

const Vector3 Enemy::GetHipWorldPosition()
{
	//モデルコンポーネントを取得
    ModelComponent* modelComponent = GetComponent<ModelComponent>();

	//腰のジョイントのワールドトランスフォームを取得
	WorldTransform hipWorldTransform = modelComponent->GetModel()->GetAnimation()->GetJointWorldTransform("mixamorig:Hips");

    //ワールド座標を取得しVector3に変換して返す
    return Vector3{
        hipWorldTransform.matWorld_.m[3][0],
        hipWorldTransform.matWorld_.m[3][1],
        hipWorldTransform.matWorld_.m[3][2],
    };
}