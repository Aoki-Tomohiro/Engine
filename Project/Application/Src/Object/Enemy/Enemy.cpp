#include "Enemy.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRoot.h"
#include "Application/Src/Object/Enemy/States/EnemyStateDeath.h"

void Enemy::Initialize()
{
    //基底クラスの初期化
    GameObject::Initialize();

    //コンポーネントの初期化
    InitializeTransformComponent();
    InitializeModelComponent();
    InitializeAnimatorComponent();
    InitializeColliderComponent();

    //状態の初期化
    InitializeState();

    //HPスプライトの生成
    InitializeHPSprites();

    //環境変数の設定
    ConfigureGlobalVariables();
}

void Enemy::Update()
{
    //タイトルシーンにいない場合
    if (!isInTitleScene_)
    {
        //状態の更新
        state_->Update();
    }

    //アニメーション後の座標を代入
    preAnimationHipPosition_ = GetHipLocalPosition();

    //回転処理
    UpdateRotation();

    //コライダーの更新
    UpdateCollider();

    //移動制限の処理
    RestrictEnemyMovement(100.0f);

    //HPの更新
    UpdateHP();

    //死亡状態かどうかを確認する
    CheckAndTransitionToDeath();

    //デバッグ時の更新処理
    DebugUpdate();

    //基底クラスの更新
    GameObject::Update();

    //環境変数の適用
    ApplyGlobalVariables();

    //ImGuiの更新
    UpdateImGui();
}

void Enemy::Draw(const Camera& camera)
{
    //基底クラスの描画
    GameObject::Draw(camera);
}

void Enemy::DrawUI()
{
    //体力バーの描画
    for (int32_t i = 0; i < hpBarSegments_.size(); ++i)
    {
        for (int32_t j = 0; j < hpBarSegments_[i].size(); ++j)
        {
            hpBarSegments_[i][j]->Draw();
        }
    }
}

void Enemy::OnCollision(GameObject* gameObject)
{
    //状態の衝突判定処理
    state_->OnCollision(gameObject);
}

void Enemy::OnCollisionEnter(GameObject* gameObject)
{
}

void Enemy::OnCollisionExit(GameObject* gameObject)
{
}

void Enemy::Reset()
{
    //フラグのリセット
    isDead_ = false;
    isGameFinished_ = false;

    //HPをリセット
    hp_ = kMaxHp;

    //タイムスケールをリセット
    timeScale_ = 1.0f;

    //Quaternionの初期化
    destinationQuaternion_ = transform_->worldTransform_.quaternion_;

    //タイトルシーンにいた場合はアニメーションブレンドを無効にする
    bool isAnimationBlending = isInTitleScene_ ? false : true;
    SetIsAnimationBlending(isAnimationBlending);

    //アニメーションの再生
    PlayAnimation("Idle", 1.0f, true);

    //状態の初期化
    InitializeState();
}

void Enemy::ChangeState(IEnemyState* newState)
{
    //新しい状態の設定
    newState->SetEnemy(this);

    //新しい状態の初期化
    newState->Initialize();

    //現在の状態を新しい状態に更新
    state_.reset(newState);
}

void Enemy::Move(const Vector3& velocity)
{
    TransformComponent* transformComponent = GetComponent<TransformComponent>();
    transformComponent->worldTransform_.translation_ += velocity * GameTimer::GetDeltaTime();
}

void Enemy::Rotate(const Vector3& vector)
{
    Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, vector));
    float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, vector);
    destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
}

void Enemy::ApplyParametersToWeapon(const CombatPhase& combatPhase)
{
    //武器を取得
    Weapon* weapon = GameObjectManager::GetInstance()->GetMutableGameObject<Weapon>("EnemyWeapon");

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

void Enemy::CheckAndTransitionToDeath()
{
    //敵の体力が0を下回っていた場合
    if (hp_ <= 0.0f)
    {
        //死亡状態に遷移
        if (!isDead_)
        {
            ChangeState(new EnemyStateDeath());
        }

        //死亡フラグを立てる
        isDead_ = true;
    }
}

void Enemy::ApplyKnockback()
{
    if (knockbackSettings_.knockbackDuration > 0.0f)
    {
        //加速度を加算する前の速度を保存
        Vector3 currentVelocity = knockbackSettings_.knockbackVelocity;

        //重力加速度を考慮
        Vector3 gravity = { 0.0f, gravityAcceleration, 0.0f };
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

void Enemy::CorrectAnimationOffset()
{
    Vector3 hipPositionOffset = GetHipLocalPosition() - preAnimationHipPosition_;
    hipPositionOffset.y = 0.0f;
    SetPosition(GetPosition() - hipPositionOffset);
}

void Enemy::PlayAnimation(const std::string& name, const float speed, const bool loop)
{
    animator_->PlayAnimation(name, speed, loop);
}

void Enemy::StopAnimation()
{
    animator_->StopAnimation();
}

void Enemy::SetIsAnimationBlending(bool isBlending)
{
    animator_->SetIsBlending(isBlending);
}

void Enemy::SetBlendDuration(const float blendDuration)
{
    animator_->SetBlendDuration(blendDuration);
}

float Enemy::GetCurrentAnimationTime()
{
    return animator_->GetCurrentAnimationTime();
}

float Enemy::GetNextAnimationTime()
{
    return animator_->GetNextAnimationTime();
}

float Enemy::GetCurrentAnimationDuration()
{
    return animator_->GetCurrentAnimationDuration();
}

float Enemy::GetNextAnimationDuration()
{
    return animator_->GetNextAnimationDuration();
}

const bool Enemy::GetIsAnimationFinished()
{
    return animator_->GetIsAnimationFinished();
}

const bool Enemy::GetIsBlendingCompleted()
{
    return animator_->GetIsBlendingCompleted();
}

const Vector3 Enemy::GetJointWorldPosition(const std::string& jointName)
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

const Vector3 Enemy::GetHipLocalPosition()
{
    //腰のジョイントのローカル座標を計算
    Vector3 hipLocalPosition = GetHipWorldPosition() - transform_->GetWorldPosition();

    //腰のジョイントのローカル座標を返す
    return hipLocalPosition;
}

const Vector3 Enemy::GetHipWorldPosition()
{
    //ワールド座標を取得しVector3に変換して返す
    return GetJointWorldPosition("mixamorig:Hips");
}

const Vector3 Enemy::GetPosition()
{
    return transform_->worldTransform_.translation_;
}

void Enemy::SetPosition(const Vector3& position)
{
    transform_->worldTransform_.translation_ = position;
}

void Enemy::InitializeState()
{
    ChangeState(new EnemyStateRoot());
}

void Enemy::InitializeTransformComponent()
{
    //トランスフォームコンポーネントを取得
    transform_ = GetComponent<TransformComponent>();
    //回転の種類をQuaternionに設定
    transform_->worldTransform_.rotationType_ = RotationType::Quaternion;
    //Quaternionの初期化
    destinationQuaternion_ = transform_->worldTransform_.quaternion_;
}

void Enemy::InitializeModelComponent()
{
    //モデルコンポーネントを取得
    model_ = GetComponent<ModelComponent>();
    //全てのマテリアルの環境マップの映り込みの係数を設定
    for (size_t i = 0; i < model_->GetModel()->GetNumMaterials(); ++i)
    {
        model_->GetModel()->GetMaterial(i)->SetEnvironmentCoefficient(0.0f);
    }
}

void Enemy::InitializeAnimatorComponent()
{
    //アニメーターコンポーネントの初期化
    animator_ = AddComponent<AnimatorComponent>();
    animator_->AddAnimation("Idle", AnimationManager::Create("Enemy/Animations/Idle.gltf"));
    animator_->AddAnimation("Walk1", AnimationManager::Create("Enemy/Animations/Walk1.gltf"));
    animator_->AddAnimation("Walk2", AnimationManager::Create("Enemy/Animations/Walk2.gltf"));
    animator_->AddAnimation("Walk3", AnimationManager::Create("Enemy/Animations/Walk3.gltf"));
    animator_->AddAnimation("Walk4", AnimationManager::Create("Enemy/Animations/Walk4.gltf"));
    animator_->AddAnimation("Run", AnimationManager::Create("Enemy/Animations/Run.gltf"));
    animator_->AddAnimation("Dash", AnimationManager::Create("Enemy/Animations/Dash.gltf"));
    animator_->AddAnimation("TackleAttack", AnimationManager::Create("Enemy/Animations/Tackle.gltf"));
    animator_->AddAnimation("JumpAttack", AnimationManager::Create("Enemy/Animations/JumpAttack.gltf"));
    animator_->AddAnimation("ComboAttack", AnimationManager::Create("Enemy/Animations/ComboAttack.gltf"));
    animator_->AddAnimation("EarthSpike", AnimationManager::Create("Enemy/Animations/EarthSpike.gltf"));
    animator_->AddAnimation("LaserBeam", AnimationManager::Create("Enemy/Animations/LaserBeam.gltf"));
    animator_->AddAnimation("HitStun", AnimationManager::Create("Enemy/Animations/HitStun.gltf"));
    animator_->AddAnimation("Knockdown", AnimationManager::Create("Enemy/Animations/Knockdown.gltf"));
    animator_->AddAnimation("StandUp", AnimationManager::Create("Enemy/Animations/StandUp.gltf"));
    animator_->AddAnimation("Death", AnimationManager::Create("Enemy/Animations/Death.gltf"));
    PlayAnimation("Idle", 1.0f, true);
}

void Enemy::InitializeColliderComponent()
{
    //コライダーコンポーネントを取得
    collider_ = GetComponent<AABBCollider>();
}

void Enemy::InitializeHPSprites()
{
    //テクスチャの名前
    std::vector<std::vector<std::string>> textureNames = {
        {"barBack_horizontalLeft.png","barBack_horizontalMid.png","barBack_horizontalRight.png"},
        {"barBlue_horizontalLeft.png","barBlue_horizontalBlue.png","barBlue_horizontalRight.png"},
    };

    //テクスチャの読み込みとスプライトの生成
    for (int32_t i = 0; i < hpBarSegments_.size(); ++i)
    {
        for (int32_t j = 0; j < hpBarSegments_[i].size(); ++j)
        {
            TextureManager::Load(textureNames[i][j]);
            hpBarSegments_[i][j].reset(Sprite::Create(textureNames[i][j], hpBarSegmentPositions_[i][j]));
        }
    }
    hpBarSegments_[0][1]->SetTextureSize(hpBarSegmentTextureSize_);
}

void Enemy::UpdateHealthBar()
{
    const int kFrontHpBarIndex = 1;
    const int kBackHpBarIndex = 0;
    const int kLeftSegmentIndex = 0;
    const int kMiddleSegmentIndex = 1;
    const int kRightSegmentIndex = 2;

    //体力バーの中央部分のサイズを更新
    UpdateHpBarMiddleSegment(kFrontHpBarIndex, kMiddleSegmentIndex);

    //体力バーの右側部分の位置を調整
    UpdateHpBarRightSegmentPosition(kFrontHpBarIndex, kMiddleSegmentIndex, kRightSegmentIndex);
}

void Enemy::UpdateHpBarMiddleSegment(int hpBarIndex, int middleSegmentIndex)
{
    //現在の体力バーのサイズを計算して設定
    Vector2 currentHpSize = { hpBarSegmentTextureSize_.x * (hp_ / kMaxHp), hpBarSegmentTextureSize_.y };
    hpBarSegments_[hpBarIndex][middleSegmentIndex]->SetTextureSize(currentHpSize);
}

void Enemy::UpdateHpBarRightSegmentPosition(int hpBarIndex, int middleSegmentIndex, int rightSegmentIndex)
{
    //右の体力バーの位置を調整
    Vector2 currentRightHpBarSegmentPosition = hpBarSegments_[hpBarIndex][middleSegmentIndex]->GetPosition();
    currentRightHpBarSegmentPosition.x += hpBarSegments_[hpBarIndex][middleSegmentIndex]->GetTextureSize().x;
    hpBarSegments_[hpBarIndex][rightSegmentIndex]->SetPosition(currentRightHpBarSegmentPosition);
}

void Enemy::UpdateRotation()
{
    //現在のクォータニオンと目的のクォータニオンを補間
    if (!isInTitleScene_)
    {
        transform_->worldTransform_.quaternion_ = Mathf::Normalize(Mathf::Slerp(transform_->worldTransform_.quaternion_, destinationQuaternion_, quaternionInterpolationSpeed_));
    }
}

void Enemy::UpdateCollider()
{
    //腰のジョイントの位置を取得
    Vector3 hipPosition = GetHipLocalPosition();

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

void Enemy::RestrictEnemyMovement(float moveLimit)
{
    //敵の現在位置から原点までの距離を計算
    float distance = Mathf::Length(transform_->worldTransform_.translation_);

    //距離が移動制限を超えているかどうかを確認
    if (distance > moveLimit) {
        //スケールを計算して移動制限範囲に収めるよう位置を調整
        float scale = moveLimit / distance;
        transform_->worldTransform_.translation_ *= scale;
    }
}

void Enemy::UpdateHP()
{
    //HPが0を下回らないようにする
    hp_ = std::max<float>(hp_, 0.0f);

    //体力バーの更新
    UpdateHealthBar();
}

void Enemy::DebugUpdate()
{
    //デバッグ状態の場合
    if (isDebug_)
    {
        //アニメーションの時間を設定
        animator_->PauseAnimation();
        animator_->SetAnimationTime(currentAnimationName_, animationTime_);
    }
    else
    {
        animator_->ResumeAnimation();
    }
}

void Enemy::ConfigureGlobalVariables()
{
    //環境変数の設定
    GlobalVariables* globalVariables = GlobalVariables::GetInstance();
    const char* groupName = "Enemy";
    globalVariables->CreateGroup(groupName);
    globalVariables->AddItem(groupName, "ColliderOffset", colliderOffset_);
}

void Enemy::ApplyGlobalVariables()
{
    GlobalVariables* globalVariables = GlobalVariables::GetInstance();
    const char* groupName = "Enemy";
    colliderOffset_ = globalVariables->GetVector3Value(groupName, "ColliderOffset");
}

void Enemy::UpdateImGui()
{
    //ImGuiのウィンドウを開始
    ImGui::Begin("Enemy");

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
            for (const auto& animation : animationName_)
            {
                //現在のアニメーション名とリストのアニメーションが一致するか確認
                bool isSelected = (currentAnimationName_ == animation);

                //アニメーション名を選択できる項目として表示
                if (ImGui::Selectable(animation.c_str(), isSelected))
                {
                    //選択されたアニメーション名を現在のアニメーション名として設定
                    currentAnimationName_ = animation;
                    animator_->SetIsBlending(false);
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