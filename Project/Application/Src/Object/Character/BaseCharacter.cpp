/**
 * @file BaseCharacter.cpp
 * @brief キャラクターの基底クラスを管理するファイル
 * @author 青木智滉
 * @date
 */

#include "BaseCharacter.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Magic/Magic.h"

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
    if (newStateName == "Stun" || !nextState_)
    {
        nextState_ = std::unique_ptr<AbstractCharacterState>(characterStateFactory_->CreateCharacterState(name_, newStateName));
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
    KnockbackParameters knockbackParameters = attackEvent->knockbackParameters;
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
        //ダメージを食らった時の処理を実行
        ApplyDamageAndKnockback(magic->GetKnockbackParameters(), magic->GetDamage(), transitionToStun ? magic->GetMagicType() == Magic::MagicType::kCharged : false);
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

    //オーディオ用のマップ
    std::map<std::string, std::string> audioFiles{};

    //ファイルパス
    const std::string kFilePath = "Application/Resources/Config/Sounds/" + name_ + "Sounds.csv";

    //オーディオCSVを読み込む
    LoadResourceFromCSV(kFilePath, audioFiles);

    //デフォルトのオーディオハンドルを追加
    audioHandles_["None"] = 0;

    //オーディオファイルをロード
    for (const auto& [key, fileName] : audioFiles)
    {
        audioHandles_[key] = audio_->LoadAudioFile(fileName);
    }
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

    //アニメーション用のマップ
    std::map<std::string, std::string> animations{};

    //ファイルパス
    const std::string kFilePath = "Application/Resources/Config/Animations/" + name_ + "Animations.csv";

    //オーディオCSVを読み込む
    LoadResourceFromCSV(kFilePath, animations);

    //読み込んだアニメーションを追加
    for (const auto& [name, path] : animations)
    {
        animator_->AddAnimation(name, AnimationManager::Create(path));
    }

    //通常アニメーションを再生
    animator_->PlayAnimation("Idle", 1.0f, true);
}

void BaseCharacter::InitializeCollider()
{
    //コライダーコンポーネントを取得
    collider_ = GetComponent<AABBCollider>();
}

void BaseCharacter::InitializeUISprites()
{
    //UIマネージャーが設定されていなければ処理を飛ばす
    if (!uiManager_) return;

    //体力バーの真ん中のスプライトを取得
    hpBarMidSprite_ = uiManager_->GetUI<DynamicUI>(name_ + "HealthBarForegroundMiddle");

    //自動でゲージを減らさないようにする
    hpBarMidSprite_->SetAutoDecrement(false);

    //残り時間を最大HPに設定
    hpBarMidSprite_->SetTimeRemaining(kMaxHp_);

    //経過時間もリセット
    hpBarMidSprite_->SetElapsedTime(kMaxHp_);

    //体力バーの右のスプライトを取得
    hpBarRightSprite_ = uiManager_->GetUI<UIElement>(name_ + "HealthBarForegroundRight");
}

void BaseCharacter::InitializeGlobalVariables()
{
    //環境変数のインスタンスを取得
    GlobalVariables* globalVariables = GlobalVariables::GetInstance();
    globalVariables->CreateGroup(name_);
    globalVariables->AddItem(name_, "ColliderOffset", colliderOffset_);
    globalVariables->AddItem(name_, "OriginOffset", transform_->worldTransform_.originOffset_);
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
    if (transform_->worldTransform_.translation_.y < GetAdjustGroundLevel())
    {
        transform_->worldTransform_.translation_.y = GetAdjustGroundLevel();
    }
}

void BaseCharacter::UpdateHP()
{
    //隙間を調整するための定数
    static const float kHpBarAlignmentOffset = 1.0f;

    //体力バーのUIが存在しなければ処理を飛ばす
    if (!hpBarMidSprite_ || !hpBarRightSprite_) return;

    //HPが0を下回らないようにする
    hp_ = std::max<float>(hp_, 0.0f);

    //体力バーの真ん中の進行状況を現在のHPに設定
    hpBarMidSprite_->SetElapsedTime(hp_);

    //右側のバーがズレないように一度更新をする
    hpBarMidSprite_->Update();

    //体力バーの右側の位置を設定
    Vector2 hpBarRightPosition = hpBarMidSprite_->GetPosition();

    //真ん中の体力バーのテクスチャサイズ分ずらす
    hpBarRightPosition.x += hpBarMidSprite_->GetSprite()->GetTextureSize().x * hpBarMidSprite_->GetScale().x - kHpBarAlignmentOffset;

    //UIの位置を設定
    hpBarRightSprite_->SetPosition(hpBarRightPosition);
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

void BaseCharacter::LoadResourceFromCSV(const std::string& filePath, std::map<std::string, std::string>& resourceMap)
{
    //リソースファイルを読み込む
    std::ifstream file(filePath);
    assert(file.is_open());

    //1行分の文字列を入れる変数
    std::string line{};

    //リソースデータを読み込む
    while (std::getline(file, line))
    {
        //"//"から始まる行またはキーやファイル名が設定されていなければ処理を飛ばす
        if (line.find("//") == 0 || line.empty()) continue;

        //1行分の文字列をストリームに変換して解析しやすくする
        std::stringstream lineStream(line);

        //音声のキーと値を入れる変数
        std::string key{}, value{};

        //音声のキーを読み取る
        std::getline(lineStream, key, ',');

        //ファイル名を読み取る
        std::getline(lineStream, value, ',');

        //キーか値が空ならスキップ
        if (key.empty() || value.empty()) continue;

        //マップに追加
        resourceMap[key] = value;
    }

    //ファイルを閉じる
    file.close();
}

void BaseCharacter::ApplyGlobalVariables()
{
    //環境変数を適用
    GlobalVariables* globalVariables = GlobalVariables::GetInstance();
    colliderOffset_ = globalVariables->GetVector3Value(name_, "ColliderOffset");
    transform_->worldTransform_.originOffset_ = globalVariables->GetVector3Value(name_, "OriginOffset");
}