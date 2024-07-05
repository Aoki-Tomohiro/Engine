#include "EnemyStateTackle.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Application//Src/Object/Enemy/Enemy.h"
#include "EnemyStateIdle.h"

void EnemyStateTackle::Initialize()
{
    //アニメーションの設定
    ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
    modelComponent->SetAnimationName("Tackle");
    modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
    modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);
}

void EnemyStateTackle::Update()
{
    //アニメーションが終わったら通常状態に戻す
    ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
    if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
    {
        enemy_->ChangeState(new EnemyStateIdle());
        modelComponent->SetAnimationName("Idle");
    }
}

void EnemyStateTackle::Draw(const Camera& camera)
{

}