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

    //チャージタイマーを進める
    attackTimer_ += GameTimer::GetDeltaTime() * enemy_->timeScale_;

    //チャージが終ったら攻撃判定を付ける
    if (attackTimer_ >= timeToActivateHitbox_ && attackTimer_ < timeToDeactivateHitbox_)
    {
        isAttack_ = true;
    }
    //攻撃が終っていたら攻撃判定をなくす
    else if (attackTimer_ >= timeToDeactivateHitbox_)
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