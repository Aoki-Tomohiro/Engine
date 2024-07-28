#include "EnemyStateDead.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Enemy/Enemy.h"

void EnemyStateDead::Initialize()
{
    //アニメーションの設定
    ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
    modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.006");
    modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
    modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);
}

void EnemyStateDead::Update()
{
    //アニメーションが終っていたら
    ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
    if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
    {
        isDissolveEnable_ = true;
    }

    if (isDissolveEnable_)
    {
        //死亡タイマーを進める
        deathTimer_ += GameTimer::GetDeltaTime();

        //徐々にDissolveを掛ける
        float dissolveParameter = 1.0f * (deathTimer_ / enemy_->deadParameters_.deathTime);
        modelComponent->GetModel()->GetMaterial(0)->SetDissolveThreshold(dissolveParameter);
        modelComponent->GetModel()->GetMaterial(0)->SetEdgeWidth(enemy_->deadParameters_.edgeWidth);
        modelComponent->GetModel()->GetMaterial(1)->SetDissolveThreshold(dissolveParameter);
        modelComponent->GetModel()->GetMaterial(1)->SetEdgeWidth(enemy_->deadParameters_.edgeWidth);

        //死亡タイマーが一定値を超えたら死亡させる
        if (deathTimer_ > enemy_->deadParameters_.deathTime)
        {
            enemy_->isDead_ = true;
        }
    }
}

void EnemyStateDead::Draw(const Camera& camera)
{
}