#include "BaseCharacter.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Magic/Magic.h"
#include "Application/Src/Object/Laser/Laser.h"
#include "Application/Src/Object/Pillar/Pillar.h"

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

    //UIスプライトの初期化
    InitializeUISprites();
}

void BaseCharacter::Update()
{
    //アニメーション後の座標を代入
    preAnimationHipPosition_ = GetJointLocalPosition("mixamorig:Hips");

    //回転処理
    UpdateRotation();

    //コライダーの更新
    UpdateCollider();

    //移動制限の処理
    RestrictMovement();

    //モデルシェイクを適用
    ApplyModelShake();

    //HPの更新
    UpdateHP();

    //死亡状態かどうかを確認する
    CheckAndTransitionToDeath();

    //デバッグの更新
    UpdateDebug();

    //ImGuiの更新
    UpdateImGui();

    //基底クラスの更新
    GameObject::Update();
}

void BaseCharacter::DrawUI()
{
    //体力バーの描画
    for (int32_t i = 0; i < hpSprites_.size(); ++i)
    {
        for (int32_t j = 0; j < hpSprites_[i].size(); ++j)
        {
            hpSprites_[i][j]->Draw();
        }
    }
}

void BaseCharacter::Move(const Vector3& velocity)
{
    transform_->worldTransform_.translation_ += velocity * GameTimer::GetDeltaTime();
}

void BaseCharacter::Rotate(const Vector3& vector)
{
    //回転軸を計算
    Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, vector));

    //角度を計算
    float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, vector);

    //クォータニオンを計算
    destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
}

void BaseCharacter::ApplyKnockback()
{
    //加速度を加算する前の速度を保存
    Vector3 currentVelocity = knockbackSettings_.knockbackVelocity;

    //重力加速度を考慮
    Vector3 gravity = { 0.0f, gravityAcceleration_, 0.0f };
    knockbackSettings_.knockbackVelocity += (gravity + knockbackSettings_.knockbackAcceleration) * GameTimer::GetDeltaTime();

    //移動処理
    Move(knockbackSettings_.knockbackVelocity);

    //ノックバックの速度の符号が変わっていたら停止させる
    if ((currentVelocity.z >= 0.0f && knockbackSettings_.knockbackVelocity.z < 0.0f) || (currentVelocity.z < 0.0f && knockbackSettings_.knockbackVelocity.z >= 0.0f))
    {
        knockbackSettings_.knockbackVelocity = { 0.0f, knockbackSettings_.knockbackVelocity.y, 0.0f };
        knockbackSettings_.knockbackAcceleration = { 0.0f, knockbackSettings_.knockbackAcceleration.y, 0.0f };
    }
}

void BaseCharacter::ApplyParametersToWeapon(const std::string& weaponName, const CombatPhase& combatPhase)
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

void BaseCharacter::CorrectAnimationOffset()
{
    Vector3 hipPositionOffset = GetJointLocalPosition("mixamorig:Hips") - preAnimationHipPosition_;
    hipPositionOffset.y = 0.0f;
    SetPosition(GetPosition() - hipPositionOffset);
}

void BaseCharacter::ProcessCollisionImpact(GameObject* gameObject, const bool transitionToStun)
{
    //衝突相手が武器だった場合
    if (Weapon* weapon = dynamic_cast<Weapon*>(gameObject))
    {
        //ダメージを食らった時の処理を実行
        ApplyDamageAndKnockback(weapon->GetKnockbackSettings(), weapon->GetDamage(), transitionToStun);
    }
    //衝突相手が魔法だった場合
    else if (Magic* magic = dynamic_cast<Magic*>(gameObject))
    {
        ApplyDamageAndKnockback(magic->GetKnockbackSettings(), magic->GetDamage(), magic->GetMagicType() == Magic::MagicType::kCharged);
    }
    //衝突相手がレーザーだった場合
    else if (Laser* laser = dynamic_cast<Laser*>(gameObject))
    {
        //ダメージを食らった処理を実行
        ApplyDamageAndKnockback(KnockbackSettings{}, laser->GetDamage(), transitionToStun);
    }
    //衝突相手が柱だった場合
    else if (Pillar* pillar = dynamic_cast<Pillar*>(gameObject))
    {
        //ダメージを食らった処理を実行
        ApplyDamageAndKnockback(KnockbackSettings{}, pillar->GetDamage(), transitionToStun);
    }
}

void BaseCharacter::ApplyDamageAndKnockback(const KnockbackSettings& knockbackSettings, const float damage, const bool transitionToStun)
{
    //ノックバックの速度を設定
    knockbackSettings_ = knockbackSettings;

    //HPを減らす
    hp_ -= damage;

    //モデルシェイクを有効にする
    StartModelShake();

    //スタン状態に遷移するかどうかをチェック
    if (transitionToStun)
    {
        TransitionToStunState();
    }
}

void BaseCharacter::StartModelShake()
{
    modelShake_.isActive = true;
    modelShake_.elapsedTime = 0.0f;
    modelShake_.originalPosition = GetPosition();
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
    return GetJointWorldPosition("mixamorig:Hips") - transform_->GetWorldPosition();
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

void BaseCharacter::InitializeUISprites()
{
    //テクスチャの読み込みとスプライトの生成
    for (int32_t i = 0; i < hpSprites_.size(); ++i)
    {
        for (int32_t j = 0; j < hpSprites_[i].size(); ++j)
        {
            TextureManager::Load(hpTextureNames_[i][j]);
            hpSprites_[i][j].reset(Sprite::Create(hpTextureNames_[i][j], hpBarSegmentPositions_[i][j]));
        }
    }
    hpSprites_[kBack][kMiddle]->SetTextureSize(hpBarSegmentTextureSize_);
}

void BaseCharacter::UpdateRotation()
{
    //現在のクォータニオンを補間
    if (!isInTitleScene_)
    {
        transform_->worldTransform_.quaternion_ = Mathf::Normalize(Mathf::Slerp(transform_->worldTransform_.quaternion_, destinationQuaternion_, quaternionInterpolationSpeed_));
    }
}

void BaseCharacter::UpdateCollider()
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

void BaseCharacter::RestrictMovement()
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

void BaseCharacter::UpdateHP()
{
    //HPが0を下回らないようにする
    hp_ = std::max<float>(hp_, 0.0f);

    //体力バーの中央部分のサイズを更新
    Vector2 currentHpSize = { hpBarSegmentTextureSize_.x * (hp_ / maxHp_), hpBarSegmentTextureSize_.y };
    hpSprites_[kFront][kMiddle]->SetTextureSize(currentHpSize);

    //体力バーの右側部分の位置を調整
    Vector2 currentRightHpBarSegmentPosition = hpSprites_[kFront][kMiddle]->GetPosition();
    currentRightHpBarSegmentPosition.x += hpSprites_[kFront][kMiddle]->GetTextureSize().x;
    hpSprites_[kFront][kRight]->SetPosition(currentRightHpBarSegmentPosition);
}

void BaseCharacter::CheckAndTransitionToDeath()
{
    //敵の体力が0を下回っていた場合
    if (hp_ <= 0.0f)
    {
        //死亡状態に遷移
        if (!isDead_)
        {
            TransitionToDeathState();
        }

        //死亡フラグを立てる
        isDead_ = true;
    }
}

void BaseCharacter::UpdateDebug()
{
    //デバッグ状態が変わった場合のみ処理を行う
    if (isDebug_ != wasDebugActive_)
    {
        //現在のデバッグの状態に応じてアニメーションを設定
        isDebug_ ? animator_->PauseAnimation() : animator_->ResumeAnimation();

        //現在のデバッグ状態を更新
        wasDebugActive_ = isDebug_;
    }

    //アニメーションの時間を設定
    if (isDebug_)
    {
        animator_->SetAnimationTime(currentAnimationName_, animationTime_);
    }
}

void BaseCharacter::UpdateImGui()
{
    //ImGuiのウィンドウを開始
    ImGui::Begin(name_.c_str());

    //デバッグモードのチェックボックス
    ImGui::Checkbox("IsDebug", &isDebug_);

    //デバッグ状態の場合
    if (isDebug_)
    {
        //アニメーション時間のスライダー
        ImGui::DragFloat("AnimationTime", &animationTime_, 0.01f, 0.0f, animator_->GetAnimation(currentAnimationName_)->GetDuration());

        //アニメーション名のコンボボックス
        if (ImGui::BeginCombo("AnimationName", currentAnimationName_.c_str()))
        {
            //アニメーション名のリストを表示
            for (const auto& animation : animationNames_)
            {
                //現在のアニメーション名とリストのアニメーションが一致するか確認
                bool isSelected = (currentAnimationName_ == animation);

                //アニメーション名を選択できる項目として表示
                if (ImGui::Selectable(animation.c_str(), isSelected))
                {
                    //選択されたアニメーション名を現在のアニメーション名として設定
                    currentAnimationName_ = animation;
                    animator_->PlayAnimation(currentAnimationName_, 1.0f, true);
                }

                //選択された項目にフォーカスを設定
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }

    //ImGuiのウィンドウを終了
    ImGui::End();
}

void BaseCharacter::UpdateModelShake()
{
    //モデルシェイクの経過時間を進める
    modelShake_.elapsedTime += GameTimer::GetDeltaTime();

    //モデルシェイクの経過時間が一定値を超えていたら終了させる
    if (modelShake_.elapsedTime > modelShake_.duration)
    {
        modelShake_.isActive = false;
    }
}

void BaseCharacter::ApplyModelShake()
{
    //モデルシェイクが有効な場合
    if (modelShake_.isActive)
    {
        //モデルシェイクの強度をランダムで決める
        Vector3 intensity = {
            RandomGenerator::GetRandomFloat(-modelShake_.intensity.x,modelShake_.intensity.x),
            RandomGenerator::GetRandomFloat(-modelShake_.intensity.y,modelShake_.intensity.y),
            RandomGenerator::GetRandomFloat(-modelShake_.intensity.z,modelShake_.intensity.z),
        };

        //座標をずらす
        Vector3 currentPosition = GetPosition();
        modelShake_.originalPosition = currentPosition;
        currentPosition += intensity * GameTimer::GetDeltaTime();
        SetPosition(currentPosition);
    }
}

void BaseCharacter::ResetToOriginalPosition()
{
    if (modelShake_.isActive)
    {
        SetPosition(modelShake_.originalPosition);
    }
}