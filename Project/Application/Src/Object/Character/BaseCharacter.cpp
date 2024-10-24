#include "BaseCharacter.h"

void BaseCharacter::Initialize()
{
    //基底クラスの初期化
    GameObject::Initialize();

    //トランスフォームの初期化
    InitializeTransform();

    //モデルの初期化
    InitializeModel();

    //アニメーターの初期化
    InitializeAnimator();

    //コライダーの初期化
    InitializeCollider();

    ////UIスプライトの初期化
    //InitializeUISprites();
}

const Vector3 BaseCharacter::GetJointWorldPosition(const std::string& jointName)
{
    //ジョイントのワールドトランスフォームを取得
    WorldTransform jointWorldTransform = model_->GetModel()->GetJointWorldTransform(jointName);

    //ワールド座標を取得しVector3に変換して返す
    return Vector3{
        jointWorldTransform.matWorld_.m[3][0],
        jointWorldTransform.matWorld_.m[3][1],
        jointWorldTransform.matWorld_.m[3][2],
    };
}

const Vector3 BaseCharacter::GetJointLocalPosition(const std::string& jointName)
{
    //腰のジョイントのローカル座標を返す
    return GetJointWorldPosition(jointName) - transform_->GetWorldPosition();
}

void BaseCharacter::InitializeTransform()
{
    //トランスフォームコンポーネントを取得
    transform_ = GetComponent<TransformComponent>();
    //回転の種類をQuaternionに設定
    transform_->worldTransform_.rotationType_ = RotationType::Quaternion;
    //Quaternionの初期化
    destinationQuaternion_ = transform_->worldTransform_.quaternion_;
}

void BaseCharacter::InitializeModel()
{
    //モデルコンポーネントを取得
    model_ = GetComponent<ModelComponent>();

    //全てのマテリアルの環境マップの映り込みの係数を設定
    for (size_t i = 0; i < model_->GetModel()->GetNumMaterials(); ++i)
    {
        model_->GetModel()->GetMaterial(i)->SetEnvironmentCoefficient(0.0f);
    }
}

void BaseCharacter::InitializeAnimator()
{
    //アニメーターコンポーネントの初期化
    animator_ = AddComponent<AnimatorComponent>();
}

void BaseCharacter::InitializeCollider()
{
    //コライダーコンポーネントを取得
    collider_ = GetComponent<AABBCollider>();
}