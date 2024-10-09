#include "ICharacter.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Weapon/Weapon.h"

void ICharacter::Initialize()
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
}

void ICharacter::Update()
{
    //アニメーション後の座標を代入
    preAnimationHipPosition_ = GetJointLocalPosition("mixamorig:Hips");

    //回転処理
    UpdateRotation();

    //コライダーの更新
    UpdateCollider();

    //移動制限の処理
    RestrictMovement();

    //HPの更新
    //UpdateHP();

    //基底クラスの更新
    GameObject::Update();
}

void ICharacter::Draw(const Camera& camera)
{
    //基底クラスの描画
    GameObject::Draw(camera);
}

void ICharacter::DrawUI()
{
    ////体力バーの描画
    //for (int32_t i = 0; i < hpBarSegments_.size(); ++i)
    //{
    //    for (int32_t j = 0; j < hpBarSegments_[i].size(); ++j)
    //    {
    //        hpBarSegments_[i][j]->Draw();
    //    }
    //}
}

void ICharacter::Move(const Vector3& velocity)
{
    transform_->worldTransform_.translation_ += velocity * GameTimer::GetDeltaTime();
}

void ICharacter::Rotate(const Vector3& vector)
{
    //回転軸を計算
    Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, vector));

    //角度を計算
    float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, vector);

    //クォータニオンを計算
    destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
}

void ICharacter::ApplyKnockback()
{
    if (knockbackSettings_.knockbackDuration > 0.0f)
    {
        //加速度を加算する前の速度を保存
        Vector3 currentVelocity = knockbackSettings_.knockbackVelocity;

        //重力加速度を考慮
        Vector3 gravity = { 0.0f, gravityAcceleration_, 0.0f };
        knockbackSettings_.knockbackVelocity += (gravity + knockbackSettings_.knockbackAcceleration) * GameTimer::GetDeltaTime();

        //ノックバックの持続時間を減少
        knockbackSettings_.knockbackDuration -= GameTimer::GetDeltaTime();

        //移動処理
        Move(knockbackSettings_.knockbackVelocity);

        //ノックバックの速度の符号が変わっていたら停止させる
        bool velocityChangedSign = (currentVelocity.z >= 0.0f && knockbackSettings_.knockbackVelocity.z < 0.0f) || (currentVelocity.z < 0.0f && knockbackSettings_.knockbackVelocity.z >= 0.0f);
        if (velocityChangedSign)
        {
            //ノックバックを停止
            knockbackSettings_.knockbackVelocity = { 0.0f, 0.0f, 0.0f };
            knockbackSettings_.knockbackAcceleration = { 0.0f,0.0f,0.0f };
        }

        //持続時間が終了したらノックバックを停止
        if (knockbackSettings_.knockbackDuration <= 0.0f)
        {
            //ノックバックの設定をクリア
            knockbackSettings_ = KnockbackSettings({});
        }
    }
}

void ICharacter::ApplyParametersToWeapon(const std::string& weaponName, const CombatPhase& combatPhase)
{
    //武器を取得
    Weapon* weapon = gameObjectManager_->GetGameObject<Weapon>(weaponName);

    //ダメージを設定
    weapon->SetDamage(combatPhase.attackSettings.damage);

    //ヒットボックスを設定
    weapon->SetHitbox(combatPhase.hitbox);

    //エフェクトの設定を武器に設定
    weapon->SetEffectSettings(combatPhase.effectSettings);

    //ノックバックの設定を取得
    KnockbackSettings knockbackSettings = combatPhase.knockbackSettings;

    //ノックバック速度を設定
    Vector3 knockbackVelocity = Mathf::RotateVector({ 0.0f, 0.0f, combatPhase.knockbackSettings.knockbackVelocity.z }, destinationQuaternion_);
    knockbackVelocity.y = combatPhase.knockbackSettings.knockbackVelocity.y;
    knockbackSettings.knockbackVelocity = knockbackVelocity;

    //ノックバック加速度を設定
    Vector3 knockbackAcceleration = Mathf::RotateVector({ 0.0f, 0.0f, combatPhase.knockbackSettings.knockbackAcceleration.z }, destinationQuaternion_);
    knockbackAcceleration.y = combatPhase.knockbackSettings.knockbackAcceleration.y;
    knockbackSettings.knockbackAcceleration = knockbackAcceleration;

    //ノックバックの設定を武器に設定
    weapon->SetKnockbackSettings(knockbackSettings);
}

void ICharacter::CorrentAnimationOffset()
{
    Vector3 hipPositionOffset = GetJointLocalPosition("mixamorig:Hips") - preAnimationHipPosition_;
    hipPositionOffset.y = 0.0f;
    SetPosition(GetPosition() - hipPositionOffset);
}

void ICharacter::ProcessCollisionImpact(GameObject* gameObject, const bool transitionToStun)
{
    //衝突相手が武器だった場合
    if (Weapon* weapon = dynamic_cast<Weapon*>(gameObject))
    {
        //ダメージを食らった時の処理を実行
        ApplyDamageAndKnockback(weapon->GetKnockbackSettings(), weapon->GetDamage(), transitionToStun);
    }
}

void ICharacter::ApplyDamageAndKnockback(const KnockbackSettings& knockbackSettings, const float damage, const bool transitionToStun)
{
    //ノックバックの速度を設定
    knockbackSettings_ = knockbackSettings;

    //HPを減らす
    hp_ -= damage;
}

const Vector3 ICharacter::GetJointWorldPosition(const std::string& jointName)
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

const Vector3 ICharacter::GetJointLocalPosition(const std::string& jointName)
{
    //腰のジョイントのローカル座標を返す
    return GetJointWorldPosition("mixamorig:Hips") - transform_->GetWorldPosition();
}

void ICharacter::InitializeTransform()
{
    //トランスフォームコンポーネントを取得
    transform_ = GetComponent<TransformComponent>();
    //回転の種類をQuaternionに設定
    transform_->worldTransform_.rotationType_ = RotationType::Quaternion;
    //Quaternionの初期化
    destinationQuaternion_ = transform_->worldTransform_.quaternion_;
}

void ICharacter::InitializeModel()
{
    //モデルコンポーネントを取得
    model_ = GetComponent<ModelComponent>();

    //全てのマテリアルの環境マップの映り込みの係数を設定
    for (size_t i = 0; i < model_->GetModel()->GetNumMaterials(); ++i)
    {
        model_->GetModel()->GetMaterial(i)->SetEnvironmentCoefficient(0.0f);
    }
}

void ICharacter::InitializeAnimator()
{
    //アニメーターコンポーネントの初期化
    animator_ = AddComponent<AnimatorComponent>();
}

void ICharacter::InitializeCollider()
{
    //コライダーコンポーネントを取得
    collider_ = GetComponent<AABBCollider>();
}

void ICharacter::UpdateRotation()
{
    //現在のクォータニオンを補間
    if (!isInTitleScene_)
    {
        transform_->worldTransform_.quaternion_ = Mathf::Normalize(Mathf::Slerp(transform_->worldTransform_.quaternion_, destinationQuaternion_, quaternionInterpolationSpeed_));
    }
}

void ICharacter::UpdateCollider()
{
    //腰のジョイントの位置を取得
    Vector3 hipPosition = GetJointLocalPosition("mixamorig:Hips");

    //アニメーション補正がアクティブな場合、腰のY成分だけ補正
    if (isAnimationCorrectionActive_)
    {
        //補正フラグをリセット
        isAnimationCorrectionActive_ = false;

        //腰の位置からXとZ成分をゼロにして補正
        hipPosition = { 0.0f, hipPosition.y, 0.0f };
    }

    //コライダーの中心座標を設定
    collider_->SetCenter(hipPosition + colliderOffset_);
}

void ICharacter::RestrictMovement()
{
    //敵の現在位置から原点までの距離を計算
    float distance = Mathf::Length(transform_->worldTransform_.translation_);

    //距離が移動制限を超えているかどうかを確認
    if (distance > kMoveLimit)
    {
        //スケールを計算して移動制限範囲に収めるよう位置を調整
        float scale = kMoveLimit / distance;
        transform_->worldTransform_.translation_ *= scale;
    }
}

void ICharacter::UpdateHP()
{
    //HPが0を下回らないようにする
    hp_ = std::max<float>(hp_, 0.0f);

    //体力バーの中央部分のサイズを更新
    Vector2 currentHpSize = { hpBarSegmentTextureSize_.x * (hp_ / maxHp_), hpBarSegmentTextureSize_.y };
    hpBarSegments_[kFront][kMiddle]->SetTextureSize(currentHpSize);

    //体力バーの右側部分の位置を調整
    Vector2 currentRightHpBarSegmentPosition = hpBarSegments_[kFront][kMiddle]->GetPosition();
    currentRightHpBarSegmentPosition.x += hpBarSegments_[kFront][kMiddle]->GetTextureSize().x;
    hpBarSegments_[kFront][kRight]->SetPosition(currentRightHpBarSegmentPosition);
}