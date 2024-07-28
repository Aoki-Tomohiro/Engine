#include "PlayerStateDead.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/Player.h"

void PlayerStateDead::Initialize()
{
    //アニメーションの設定
    ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
    modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.031");
    modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
    modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);
}

void PlayerStateDead::Update()
{
    //アニメーションが終っていたら
    ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
    if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
    {
        isDissolveEnable_ = true;
    }

    if (isDissolveEnable_)
    {
        //死亡タイマーを進める
        deathTimer_ += GameTimer::GetDeltaTime();

        //徐々にDissolveを掛ける
        float dissolveParameter = 1.0f * (deathTimer_ / player_->deadParameters_.deathTime);
        modelComponent->GetModel()->GetMaterial(0)->SetDissolveThreshold(dissolveParameter);
        modelComponent->GetModel()->GetMaterial(1)->SetDissolveThreshold(dissolveParameter);
        modelComponent->GetModel()->GetMaterial(2)->SetDissolveThreshold(dissolveParameter);
        modelComponent->GetModel()->GetMaterial(3)->SetDissolveThreshold(dissolveParameter);
        modelComponent->GetModel()->GetMaterial(4)->SetDissolveThreshold(dissolveParameter);
        modelComponent->GetModel()->GetMaterial(5)->SetDissolveThreshold(dissolveParameter);
        modelComponent->GetModel()->GetMaterial(6)->SetDissolveThreshold(dissolveParameter);

        //死亡タイマーが一定値を超えたら死亡させる
        if (deathTimer_ > player_->deadParameters_.deathTime)
        {
            player_->isDead_ = true;
        }
    }
}

void PlayerStateDead::Draw(const Camera& camera)
{
}

void PlayerStateDead::OnCollision(GameObject* other)
{
}

void PlayerStateDead::OnCollisionEnter(GameObject* other)
{
}

void PlayerStateDead::OnCollisionExit(GameObject* other)
{
}