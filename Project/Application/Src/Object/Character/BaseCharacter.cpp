#include "BaseCharacter.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Magic/Magic.h"
#include "Application/Src/Object/Laser/Laser.h"
#include "Application/Src/Object/Pillar/Pillar.h"

void BaseCharacter::Initialize()
{
    //基底クラスの初期化
    GameObject::Initialize();

    //アクションマップの初期化
    InitializeActionMap();

    //オーディオの初期化
    InitializeAudio();

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

    //武器の初期化
    InitializeWeapon();

    //環境変数の初期化
    InitializeGlobalVariables();

    //CharacterStateFactoryのインスタンスを取得
    characterStateFactory_ = CharacterStateFactory::GetInstance();
}

void BaseCharacter::Update()
{
    //モデルシェイクでずれた分の座標をリセット
    ResetToOriginalPosition();

    //モデルシェイクの更新
    UpdateModelShake();

    //新しい状態に遷移
    TransitionToNextState();

    //状態の更新
    if (currentState_)
    {
        currentState_->Update();
    }

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

    //環境変数の適用
    ApplyGlobalVariables();

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

void BaseCharacter::ApplyDamageAndKnockback(const KnockbackParameters& knockbackParameters, const float damage, const bool transitionToStun)
{
    //ノックバックの速度を設定
    knockbackParameters_ = knockbackParameters;

    //HPを減らす
    hp_ -= damage;

    //モデルシェイクを有効にする
    StartModelShake();

    //スタン状態に遷移するかどうかをチェック
    if (transitionToStun)
    {
        //スタン状態のフラグを立てる
        isStunTriggered_ = true;
        //スタン状態に遷移
        ChangeState("Stun");
    }
}

bool BaseCharacter::ChangeState(const std::string& newStateName)
{
    //新しい状態を生成して次の状態に設定
    if (!nextState_)
    {
        nextState_ = std::unique_ptr<ICharacterState>(characterStateFactory_->CreateCharacterState(name_, newStateName));
        return true;
    }
    return false;
}

void BaseCharacter::Move(const Vector3& velocity)
{
    //移動処理
    transform_->worldTransform_.translation_ += velocity * GameTimer::GetDeltaTime() * timeScale_;
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
    Vector3 currentVelocity = knockbackParameters_.velocity;

    //重力加速度を考慮
    Vector3 gravity = { 0.0f, gravityAcceleration_, 0.0f };
    knockbackParameters_.velocity += (gravity + knockbackParameters_.acceleration) * GameTimer::GetDeltaTime() * timeScale_;

    //移動処理
    Move(knockbackParameters_.velocity);

    //ノックバックの速度の符号が変わっていたら停止させる
    if ((currentVelocity.z >= 0.0f && knockbackParameters_.velocity.z < 0.0f) || (currentVelocity.z < 0.0f && knockbackParameters_.velocity.z >= 0.0f))
    {
        knockbackParameters_.velocity = { 0.0f, knockbackParameters_.velocity.y, 0.0f };
        knockbackParameters_.acceleration = { 0.0f, knockbackParameters_.acceleration.y, 0.0f };
    }
}

void BaseCharacter::ApplyParametersToWeapon(const AttackEvent* attackEvent)
{
    //ダメージを設定
    weapon_->SetDamage(attackEvent->attackParameters.damage);

    //ヒットボックスのパラメーターを設定
    weapon_->SetHitboxParameters(attackEvent->hitboxParameters);

    //ノックバックのパラメーターを設定
    KnockbackParameters knockbackParameters{};
    knockbackParameters.velocity = Mathf::RotateVector(attackEvent->knockbackParameters.velocity, transform_->worldTransform_.quaternion_);
    knockbackParameters.acceleration = Mathf::RotateVector(attackEvent->knockbackParameters.acceleration, transform_->worldTransform_.quaternion_);
    knockbackParameters.reactionType = attackEvent->knockbackParameters.reactionType;
    weapon_->SetKnockbackParameters(knockbackParameters);
}

void BaseCharacter::ProcessCollisionImpact(GameObject* gameObject, const bool transitionToStun)
{
    //衝突相手が武器だった場合
    if (Weapon* weapon = dynamic_cast<Weapon*>(gameObject))
    {
        //ダメージを食らった時の処理を実行
        ApplyDamageAndKnockback(weapon->GetKnockbackParameters(), weapon->GetDamage(), transitionToStun);
    }
    //衝突相手が魔法だった場合
    else if (Magic* magic = dynamic_cast<Magic*>(gameObject))
    {
        ApplyDamageAndKnockback(magic->GetKnockbackParameters(), magic->GetDamage(), transitionToStun ? magic->GetMagicType() == Magic::MagicType::kCharged : false);
    }
    //衝突相手がレーザーだった場合
    else if (Laser* laser = dynamic_cast<Laser*>(gameObject))
    {
        //ダメージを食らった処理を実行
        ApplyDamageAndKnockback(KnockbackParameters{}, laser->GetDamage(), transitionToStun);
    }
    //衝突相手が柱だった場合
    else if (Pillar* pillar = dynamic_cast<Pillar*>(gameObject))
    {
        //ダメージを食らった処理を実行
        ApplyDamageAndKnockback(KnockbackParameters{}, pillar->GetDamage(), transitionToStun);
    }
}

void BaseCharacter::StartModelShake()
{
    modelShake_.isActive = true;
    modelShake_.elapsedTime = 0.0f;
    modelShake_.originalPosition = GetPosition();
}

void BaseCharacter::PlaySoundEffect(const std::string& soundEffectType)
{
    //サウンドエフェクトが設定されていなければ飛ばす
    if (soundEffectType == "None") return;

    //指定のサウンドエフェクトを探す
    auto it = audioHandles_.find(soundEffectType);
    //指定のサウンドエフェクトが見つかった場合は再生する
    if (it != audioHandles_.end())
    {
        audio_->PlayAudio(it->second, false, 0.2f);
    }
}

void BaseCharacter::StartDebugMode(const std::string& animationName)
{
    isDebug_ = true;
    animator_->SetIsBlending(false);
    animator_->PlayAnimation(animationName, 1.0f, false);
    animator_->PauseAnimation();
}

void BaseCharacter::EndDebugMode()
{
    isDebug_ = false;
    animator_->SetIsBlending(true);
    animator_->ResumeAnimation();
}

Vector3 BaseCharacter::GetJointWorldPosition(const std::string& jointName) const
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

Vector3 BaseCharacter::GetJointLocalPosition(const std::string& jointName) const
{
    //腰のジョイントのローカル座標を返す
    return GetJointWorldPosition(jointName) - transform_->GetWorldPosition();
}

void BaseCharacter::InitializeAudio()
{
    //オーディオのインスタンスを取得
    audio_ = Audio::GetInstance();

    //デフォルトのオーディオハンドルを追加
    audioHandles_["None"] = 0;
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

void BaseCharacter::InitializeWeapon()
{
    //武器を生成
    weapon_ = gameObjectManager_->CreateGameObject<Weapon>(name_ + "Weapon");
    //キャラクターの右手に親子付け
    TransformComponent* weaponTransform = weapon_->GetComponent<TransformComponent>();
    weaponTransform->worldTransform_.SetParent(&model_->GetModel()->GetJointWorldTransform("mixamorig:RightHand"));
}

void BaseCharacter::InitializeGlobalVariables()
{
    //環境変数のインスタンスを取得
    GlobalVariables* globalVariables = GlobalVariables::GetInstance();
    globalVariables->CreateGroup(name_);
    globalVariables->AddItem(name_, "ColliderOffset", colliderOffset_);
}

void BaseCharacter::UpdateRotation()
{
    //タイトルシーンにいない場合はクォータニオンを補間
    if (!isInTitleScene_)
    {
        transform_->worldTransform_.quaternion_ = Mathf::Normalize(Mathf::Slerp(transform_->worldTransform_.quaternion_, destinationQuaternion_, quaternionInterpolationSpeed_));
    }
}

void BaseCharacter::UpdateCollider()
{
    //腰のジョイントの位置を取得
    Vector3 hipPosition = GetJointLocalPosition("mixamorig:Hips");

    //腰の位置からXとZ成分をゼロにして補正
    hipPosition = { 0.0f, hipPosition.y, 0.0f };

    //コライダーの中心座標を設定
    collider_->SetCenter(hipPosition + colliderOffset_);
}

void BaseCharacter::RestrictMovement()
{
    //敵の現在位置から原点までの距離を計算
    float distance = Mathf::Length(transform_->worldTransform_.translation_);

    //距離が移動制限を超えているかどうかを確認
    if (distance > kMoveLimit_)
    {
        //スケールを計算して移動制限範囲に収めるよう位置を調整
        float scale = kMoveLimit_ / distance;
        transform_->worldTransform_.translation_ *= scale;
    }

    //地面にキャラクターが埋まっていた場合は地面の高さに合わせる
    if (transform_->worldTransform_.translation_.y < 0.0f)
    {
        transform_->worldTransform_.translation_.y = 0.0f;
    }
}

void BaseCharacter::UpdateHP()
{
    //HPが0を下回らないようにする
    hp_ = std::max<float>(hp_, 0.0f);

    //体力バーの中央部分のサイズを更新
    Vector2 currentHpSize = { hpBarSegmentTextureSize_.x * (hp_ / kMaxHp_), hpBarSegmentTextureSize_.y };
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
            ChangeState("Death");
        }

        //死亡フラグを立てる
        isDead_ = true;
    }
}

void BaseCharacter::TransitionToNextState()
{
    if (nextState_)
    {
        //キャラクターを設定
        nextState_->SetCharacter(this);
        //新しい状態の初期化
        nextState_->Initialize();
        //現在の状態を次の状態に切り替え
        currentState_ = std::move(nextState_);
        //次の状態をリセット
        nextState_.reset();
    }
}

void BaseCharacter::UpdateModelShake()
{
    //モデルシェイクの経過時間を進める
    modelShake_.elapsedTime += GameTimer::GetDeltaTime() * timeScale_;

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
        currentPosition += intensity * GameTimer::GetDeltaTime() * timeScale_;
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

void BaseCharacter::ApplyGlobalVariables()
{
    GlobalVariables* globalVariables = GlobalVariables::GetInstance();
    colliderOffset_ = globalVariables->GetVector3Value(name_, "ColliderOffset");
}