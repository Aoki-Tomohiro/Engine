#include "EnemyStateTackle.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Components/Collision/OBBCollider.h"
#include "Engine/Components/Collision/CollisionAttributeManager.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application//Src/Object/Enemy/Enemy.h"
#include "Application//Src/Object/Enemy/States/EnemyStateIdle.h"
#include "Application//Src/Object/Warning/Warning.h"

void EnemyStateTackle::Initialize()
{
    //アニメーションの設定
    TransformComponent* enemyTransformComponent = enemy_->GetComponent<TransformComponent>();
    ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
    modelComponent->SetAnimationName("Tackle");
    modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
    modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);

    //警告用のオブジェクトを生成
    Warning* warning = GameObjectManager::GetInstance()->CreateGameObject<Warning>();
    //Transformの追加
    TransformComponent* warningTransformComponent = warning->AddComponent<TransformComponent>();
    warningTransformComponent->Initialize();
    warningTransformComponent->worldTransform_.rotationType_ = RotationType::Quaternion;
    Vector3 offset = Mathf::RotateVector({ 0.0f, 0.0f, 7.1f }, enemy_->destinationQuaternion_);
    warningTransformComponent->worldTransform_.translation_ = enemyTransformComponent->GetWorldPosition() + offset;
    warningTransformComponent->worldTransform_.quaternion_ = enemy_->destinationQuaternion_;
    warningTransformComponent->worldTransform_.scale_ = { 4.0f,0.01f,16.5f };
    //Modelの追加
    ModelComponent* warningModelComponent = warning->AddComponent<ModelComponent>();
    warningModelComponent->Initialize("Cube", Transparent);
    warningModelComponent->SetTransformComponent(warningTransformComponent);
    warningModelComponent->GetModel()->GetMaterial(1)->SetColor({ 1.0f,0.0f,0.0f,0.6f });
    //Colliderの追加
    OBBCollider* warningCollider = warning->AddComponent<OBBCollider>();
    warningCollider->Initialize();
    warningCollider->SetTransformComponent(warningTransformComponent);
    warningCollider->SetCenter({ 0.0f,4.0f,0.0f });
    warningCollider->SetSize({ 4.0f,4.0f,16.5f });
    warningCollider->SetCollisionAttribute(CollisionAttributeManager::GetInstance()->GetAttribute("Warning"));
    warningCollider->SetCollisionMask(CollisionAttributeManager::GetInstance()->GetMask("Warning"));
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
        //攻撃警告のフラグを立てる
        isWarning_ = true;
    }
    else if (currentAnimationTime > warningDuration_ && currentAnimationTime <= attackDuration_)
    {
        //攻撃フラグを立てる
        isAttack_ = true;
    }
    else if (currentAnimationTime > attackDuration_)
    {
        //警告モデルを削除
        if (Warning* warning = GameObjectManager::GetInstance()->GetGameObject<Warning>())
        {
            warning->SetIsDestroy(true);
        }
        
        //フラグの初期化
        isWarning_ = false;
        isAttack_ = false;
    }

    //アニメーションが終わっていたら
    if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
    {
        //通常状態に戻す
        modelComponent->SetAnimationName("Idle");
        enemy_->ChangeState(new EnemyStateIdle());
    }
}

void EnemyStateTackle::Draw(const Camera& camera)
{

}