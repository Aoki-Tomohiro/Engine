#include "EnemyStateTackle.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
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
    //チャージタイマーを進める
    chargeTimer_ += GameTimer::GetDeltaTime();

    //チャージが終ったら当たり判定を付ける
    ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
    if (chargeTimer_ > chargeDuration_)
    {
        isAttack_ = true;
    }
    if (chargeTimer_ > chargeDuration_ + 1.35f)
    {
        isAttack_ = false;
    }

    //アニメーションが終わったら通常状態に戻す
    if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
    {
        enemy_->ChangeState(new EnemyStateIdle());
        modelComponent->SetAnimationName("Idle");
    }
}

void EnemyStateTackle::Draw(const Camera& camera)
{

}