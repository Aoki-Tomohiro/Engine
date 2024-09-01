#include "Enemy.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Components/Animator/AnimatorComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRoot.h"
#include "Application/Src/Object/Enemy/States/EnemyStateDeath.h"

void Enemy::Initialize()
{
    //基底クラスの初期化
    GameObject::Initialize();

    //状態の初期化
    ChangeState(new EnemyStateRoot());

    //トランスフォームコンポーネントを取得
    TransformComponent* transformComponent = GetComponent<TransformComponent>();
    //回転の種類をQuaternionに設定
    transformComponent->worldTransform_.rotationType_ = RotationType::Quaternion;
    //Quaternionの初期化
    destinationQuaternion_ = transformComponent->worldTransform_.quaternion_;

    //モデルコンポーネントを取得
    ModelComponent* modelComponent = GetComponent<ModelComponent>();
    //全てのマテリアルの環境マップの映り込みの係数を設定
    for (size_t i = 0; i < modelComponent->GetModel()->GetNumMaterials(); ++i)
    {
        modelComponent->GetModel()->GetMaterial(i)->SetEnvironmentCoefficient(0.0f);
    }

    //体力のスプライトの生成
    TextureManager::Load("HpBar.png");
    hpSprite_.reset(Sprite::Create("HpBar.png", hpSpritePosition_));
    hpSprite_->SetColor({ 1.0f, 0.1f, 0.0f, 1.0f });
    TextureManager::Load("HpBarFrame.png");
    hpFrameSprite_.reset(Sprite::Create("HpBarFrame.png", hpFrameSpritePosition_));
    hpFrameSprite_->SetColor({ 1.0f, 0.1f, 0.0f, 1.0f });

    //環境変数の設定
    GlobalVariables* globalVariables = GlobalVariables::GetInstance();
    const char* groupName = "Enemy";
    globalVariables->CreateGroup(groupName);
    globalVariables->AddItem(groupName, "ColliderOffset", colliderOffset_);
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
    RestrictPlayerMovement(100.0f);

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
    //HPの描画
    hpSprite_->Draw();
    hpFrameSprite_->Draw();
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
        //現在の速度が逆向きにならないように加速度を調整
        if (Mathf::Dot(knockbackSettings_.knockbackVelocity, knockbackSettings_.knockbackAcceleration) > 0.0f)
        {
            //速度と加速度の向きが逆の場合加速度をリセット
            knockbackSettings_.knockbackAcceleration = { 0.0f,0.0f,0.0f };
        }

        //重力加速度を考慮
        Vector3 gravity = { 0.0f, gravityAcceleration, 0.0f };
        knockbackSettings_.knockbackVelocity += (gravity + knockbackSettings_.knockbackAcceleration) * GameTimer::GetDeltaTime();

        //ノックバックの持続時間を減少
        knockbackSettings_.knockbackDuration -= GameTimer::GetDeltaTime();

        //移動処理
        Move(knockbackSettings_.knockbackVelocity);

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
    AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
    animatorComponent->PlayAnimation(name, speed, loop);
}

void Enemy::StopAnimation()
{
    AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
    animatorComponent->StopAnimation();
}

void Enemy::SetIsAnimationBlending(bool isBlending)
{
    AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
    animatorComponent->SetIsBlending(isBlending);
}

float Enemy::GetCurrentAnimationTime()
{
    AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
    return animatorComponent->GetCurrentAnimationTime();
}

float Enemy::GetNextAnimationTime()
{
    AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
    return animatorComponent->GetNextAnimationTime();
}

const bool Enemy::GetIsAnimationFinished()
{
    AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
    return animatorComponent->GetIsAnimationFinished();
}

const bool Enemy::GetIsBlendingCompleted()
{
    AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
    return animatorComponent->GetIsBlendingCompleted();
}

const Vector3 Enemy::GetHipLocalPosition()
{
    //トランスフォームコンポーネントを取得
    TransformComponent* transformComponent = GetComponent<TransformComponent>();

    //腰のジョイントのローカル座標を計算
    Vector3 hipLocalPosition = GetHipWorldPosition() - transformComponent->GetWorldPosition();

    //腰のジョイントのローカル座標を返す
    return hipLocalPosition;
}

const Vector3 Enemy::GetHipWorldPosition()
{
	//モデルコンポーネントを取得
    ModelComponent* modelComponent = GetComponent<ModelComponent>();

	//腰のジョイントのワールドトランスフォームを取得
	WorldTransform hipWorldTransform = modelComponent->GetModel()->GetJointWorldTransform("mixamorig:Hips");

    //ワールド座標を取得しVector3に変換して返す
    return Vector3{
        hipWorldTransform.matWorld_.m[3][0],
        hipWorldTransform.matWorld_.m[3][1],
        hipWorldTransform.matWorld_.m[3][2],
    };
}

const Vector3 Enemy::GetPosition()
{
    TransformComponent* transformComponent = GetComponent<TransformComponent>();
    return transformComponent->worldTransform_.translation_;
}

void Enemy::SetPosition(const Vector3& position)
{
    TransformComponent* transformComponent = GetComponent<TransformComponent>();
    transformComponent->worldTransform_.translation_ = position;
}

void Enemy::UpdateRotation()
{
    //トランスフォームコンポーネントを取得
    TransformComponent* transformComponent = GetComponent<TransformComponent>();

    //現在のクォータニオンと目的のクォータニオンを補間
    transformComponent->worldTransform_.quaternion_ = Mathf::Normalize(Mathf::Slerp(transformComponent->worldTransform_.quaternion_, destinationQuaternion_, quaternionInterpolationSpeed_));
}

void Enemy::UpdateCollider()
{
    //AABBColliderを取得
    AABBCollider* collider = GetComponent<AABBCollider>();

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
    collider->SetCenter(hipPosition + colliderOffset_);
}

void Enemy::RestrictPlayerMovement(float moveLimit)
{
    //TransformComponentを取得
    TransformComponent* transformComponent = GetComponent<TransformComponent>();

    //プレイヤーの現在位置から原点までの距離を計算
    float distance = Mathf::Length(transformComponent->worldTransform_.translation_);

    //距離が移動制限を超えているかどうかを確認
    if (distance > moveLimit) {
        //スケールを計算して移動制限範囲に収めるよう位置を調整
        float scale = moveLimit / distance;
        transformComponent->worldTransform_.translation_ *= scale;
    }
}

void Enemy::UpdateHP()
{
    //HPバーの処理
    hp_ = (hp_ <= 0.0f) ? 0.0f : hp_;
    Vector2 currentHPSize = { hpSpriteSize_.x * (hp_ / kMaxHP), hpSpriteSize_.y };
    hpSprite_->SetSize(currentHPSize);
}

void Enemy::DebugUpdate()
{
    //アニメーターコンポーネントを取得
    AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();

    //デバッグ状態の場合
    if (isDebug_)
    {
        //アニメーションの時間を設定
        animatorComponent->SetAnimationTime(currentAnimationName_, animationTime_);

        //アニメーションのブレンドを無効化する
        animatorComponent->SetIsBlending(false);
    }
    else
    {
        //アニメーションのブレンドを有効化する
        animatorComponent->SetIsBlending(true);
    }
}

void Enemy::ApplyGlobalVariables()
{
    GlobalVariables* globalVariables = GlobalVariables::GetInstance();
    const char* groupName = "Enemy";
    colliderOffset_ = globalVariables->GetVector3Value(groupName, "ColliderOffset");
}

void Enemy::UpdateImGui()
{
    //アニメーターコンポーネントを取得
    AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();

    //ImGuiのウィンドウを開始
    ImGui::Begin("Enemy");

    //デバッグモードのチェックボックス
    ImGui::Checkbox("IsDebug", &isDebug_);

    //デバッグ状態の場合
    if (isDebug_)
    {
        //アニメーション時間のスライダー
        ImGui::DragFloat("AnimationTime", &animationTime_, 0.01f, 0.0f, animatorComponent->GetAnimation(currentAnimationName_)->GetDuration());

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
                    animatorComponent->PlayAnimation(currentAnimationName_, 1.0f, true);
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