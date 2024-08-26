#include "Enemy.h"
#include "Engine/Components/Model/ModelComponent.h"

void Enemy::Initialize()
{
    //基底クラスの初期化
    GameObject::Initialize();
}

void Enemy::Update()
{
    //基底クラスの更新
    GameObject::Update();
}

void Enemy::Draw(const Camera& camera)
{
    //基底クラスの描画
    GameObject::Draw(camera);
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
	WorldTransform hipWorldTransform = modelComponent->GetModel()->GetJointWorldTransform("mixamorig:Hips");

    //ワールド座標を取得しVector3に変換して返す
    return Vector3{
        hipWorldTransform.matWorld_.m[3][0],
        hipWorldTransform.matWorld_.m[3][1],
        hipWorldTransform.matWorld_.m[3][2],
    };
}