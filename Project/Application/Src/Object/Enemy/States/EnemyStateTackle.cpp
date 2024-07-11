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
    //アニメーションの速度の更新
    ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
    modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(enemy_->timeScale_);

    //デバッグのフラグが立っているとき
    if (enemy_->isDebug_)
    {
        modelComponent->GetModel()->GetAnimation()->SetAnimationTime(enemy_->animationTime_);
    }

    //現在のアニメーションの時間を取得
    float currentAnimationTime = modelComponent->GetModel()->GetAnimation()->GetAnimationTime();
    if (currentAnimationTime <= chargeDuration_)
    {

    }
    else if (currentAnimationTime > chargeDuration_ && currentAnimationTime <= warningDuration_)
    {
        isWarning_ = true;
    }
    else if (currentAnimationTime > warningDuration_ && currentAnimationTime <= attackDuration_)
    {
        isAttack_ = true;
    }
    else if (currentAnimationTime > attackDuration_)
    {
        isWarning_ = false;
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