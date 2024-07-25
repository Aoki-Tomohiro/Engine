#include "EnemyStateTackle.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRoot.h"

void EnemyStateTackle::Initialize()
{
    //アニメーションの設定
    ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
    modelComponent->SetAnimationName("Tackle");
    modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
    modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);

    //警告用のオブジェクトの設定
    Warning::BoxWarningObjectSettings warningObjectSettings{};
    TransformComponent* transformComponent = enemy_->GetComponent<TransformComponent>();
    warningObjectSettings.warningPrimitive = Warning::kBox;
    warningObjectSettings.position = transformComponent->GetWorldPosition();
    warningObjectSettings.quaternion = enemy_->destinationQuaternion_;
    warningObjectSettings.scale = { 5.0f,0.01f,17.5f };
    warningObjectSettings.offset = { 0.0f, 0.0f, 7.64f };
    warningObjectSettings.colliderCenter = { 0.0f,4.0f,0.0f };
    warningObjectSettings.colliderSize = { 5.0f,4.0f,17.5f };

    //警告用のオブジェクトを作成
    warning_ = enemy_->CreateBoxWarningObject(warningObjectSettings);
}

void EnemyStateTackle::Update()
{
    //デバッグのフラグが立っているとき
    ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
    if (enemy_->isDebug_)
    {
        modelComponent->GetModel()->GetAnimation()->SetAnimationTime(enemy_->animationTime_);
        return;
    }

    //前のフレームの状態を更新
    preTackleState_ = currentTackleState_;

    //アニメーションの速度の更新
    modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(enemy_->timeScale_);

    //現在のアニメーションの時間を取得
    float currentAnimationTime = modelComponent->GetModel()->GetAnimation()->GetAnimationTime();

    //現在のアニメーションに時間に応じて状態を変える
    if (currentAnimationTime <= enemy_->tackleParameters_.chargeDuration_)
    {
        currentTackleState_ = kCharge;
    }
    else if (currentAnimationTime <= enemy_->tackleParameters_.warningDuration_)
    {
        currentTackleState_ = kWarning;
    }
    else if (currentAnimationTime <= enemy_->tackleParameters_.attackDuration_)
    {
        currentTackleState_ = kAttacking;
    }
    else if (currentAnimationTime > enemy_->tackleParameters_.attackDuration_)
    {
        currentTackleState_ = kRecovery;
    }

    //現在の状態の処理
    switch (currentTackleState_)
    {
    case kCharge:

        break;
    case kWarning:

        //前のフレームがチャージ状態の場合パーティクルを出す
        if (preTackleState_ != currentTackleState_)
        {
            enemy_->CreateWarningParticle();
        }

        //攻撃警告のフラグを立てる
        isWarning_ = true;

        break;
    case kAttacking:

        //攻撃フラグを立てる
        isAttack_ = true;

        break;
    case kRecovery:

        //前のフレームが攻撃状態の場合、警告オブジェクトを削除
        if (preTackleState_ != currentTackleState_)
        {
            warning_->SetIsDestroy(true);
        }

        //フラグの初期化
        isWarning_ = false;
        isAttack_ = false;

        break;
    }

    // アニメーションが終わっていたら通常状態に戻す
    if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
    {
        enemy_->ChangeState(new EnemyStateRoot());
    }
}

void EnemyStateTackle::Draw(const Camera& camera)
{

}