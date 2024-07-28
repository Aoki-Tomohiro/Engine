#include "EnemyStateTackle.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRoot.h"
#include "Application/Src/Object/Enemy/States/EnemyStateDead.h"
#include <algorithm>

void EnemyStateTackle::Initialize()
{
    //アニメーションの設定
    ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
    modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.001");
    modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
    modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);

    //警告用のオブジェクトの設定
    Warning::BoxWarningObjectSettings warningObjectSettings{};
    TransformComponent* transformComponent = enemy_->GetComponent<TransformComponent>();
    warningObjectSettings.warningPrimitive = Warning::kBox;
    warningObjectSettings.position = transformComponent->worldTransform_.translation_;
    warningObjectSettings.quaternion = enemy_->destinationQuaternion_;
    warningObjectSettings.scale = { 5.0f,0.01f,14.0f };
    warningObjectSettings.offset = { 0.0f, 0.0f, 11.87f };
    warningObjectSettings.colliderCenter = { 0.0f,4.0f,0.0f };
    warningObjectSettings.colliderSize = { 5.0f,4.0f,14.0f };
    //警告用のオブジェクトを作成
    warning_ = enemy_->CreateBoxWarningObject(warningObjectSettings);

    //開始地点を設定
    startPosition_ = transformComponent->worldTransform_.translation_;
    //目標座標を設定
    Vector3 offset = { 0.0f,0.0f,enemy_->tackleParameters_.targetDistance };
    targetPosition_ = startPosition_ + Mathf::RotateVector(offset, enemy_->destinationQuaternion_);

    //ノックバック速度を設定
    enemy_->knockbackSpeed_ = enemy_->tackleParameters_.attackParameters.knockbackSpeed;
    //ダメージを設定
    enemy_->damage_ = enemy_->tackleParameters_.attackParameters.damage;
}

void EnemyStateTackle::Update()
{
    //デバッグのフラグが立っているとき
    ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
    if (enemy_->isDebug_)
    {
        modelComponent->GetModel()->GetAnimation()->SetAnimationTime(enemy_->animationTime_);
    }

    //前のフレームの状態を更新
    preTackleState_ = currentTackleState_;

    //現在のアニメーションの時間を取得
    float currentAnimationTime = modelComponent->GetModel()->GetAnimation()->GetAnimationTime();

    //現在のアニメーションに時間に応じて状態を変える
    if (currentAnimationTime <= enemy_->tackleParameters_.attackParameters.chargeDuration)
    {
        currentTackleState_ = kCharge;
    }
    else if (currentAnimationTime <= enemy_->tackleParameters_.attackParameters.warningDuration)
    {
        currentTackleState_ = kWarning;
    }
    else if (currentAnimationTime <= enemy_->tackleParameters_.attackParameters.attackDuration)
    {
        currentTackleState_ = kAttacking;
    }
    else if (currentAnimationTime > enemy_->tackleParameters_.attackParameters.attackDuration)
    {
        currentTackleState_ = kRecovery;
    }

    //TransformComponentを取得
    TransformComponent* transformComponent = enemy_->GetComponent<TransformComponent>();
    float attackDuration = enemy_->tackleParameters_.attackParameters.attackDuration - enemy_->tackleParameters_.attackParameters.warningDuration;
    float attackTime = currentAnimationTime - enemy_->tackleParameters_.attackParameters.warningDuration;
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

            //攻撃警告のフラグを立てる
            isWarning_ = true;
        }

        break;
    case kAttacking:

        //前のフレームがチャージ状態の場合パーティクルを出す
        if (preTackleState_ != currentTackleState_)
        {
            //攻撃フラグを立てる
            isAttack_ = true;

            //音声を鳴らす
            enemy_->audio_->PlayAudio(enemy_->tackleAudioHandle_, false, 0.6f);
        }

        //イージングさせる
        transformComponent->worldTransform_.translation_ = startPosition_ + (targetPosition_ - startPosition_) * Mathf::EaseOutExpo(1.0f * (attackTime / attackDuration));

        break;
    case kRecovery:

        //前のフレームが攻撃状態の場合、警告オブジェクトを削除
        if (preTackleState_ != currentTackleState_)
        {
            warning_->SetIsDestroy(true);

            //フラグの初期化
            isWarning_ = false;
            isAttack_ = false;
        }

        break;
    }

    // アニメーションが終わっていたら通常状態に戻す
    if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd() && !enemy_->isDebug_)
    {
        modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);
        enemy_->ChangeState(new EnemyStateRoot());
    }
    //HPが0を下回ったら
    else if (enemy_->hp_ <= 0.0f)
    {
        //死亡状態にする
        enemy_->ChangeState(new EnemyStateDead());
    }
}

void EnemyStateTackle::Draw(const Camera& camera)
{

}