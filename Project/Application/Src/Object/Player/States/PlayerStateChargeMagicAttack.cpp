#include "PlayerStateChargeMagicAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Player/States/PlayerStateFalling.h"

void PlayerStateChargeMagicAttack::Initialize()
{
    //オーディオのインスタンスを取得
    audio_ = Audio::GetInstance();

    //インプットのインスタンスを取得
    input_ = Input::GetInstance();

    //アニメーション設定と再生
    SetMagicAttackAnimation("MagicAttack");

    //魔法攻撃フラグのリセット
    player_->SetActionFlag(Player::ActionFlag::kChargeMagicAttackEnabled, false);

    //速度を計算
    velocity_.y = player_->GetMagicAttackParameters().verticalRetreatSpeed;

    //加速度を計算
    acceleration_ = Mathf::RotateVector({ 0.0f, 0.0f, player_->GetMagicAttackParameters().acceleration.z }, player_->GetDestinationQuaternion());
    acceleration_.y = player_->GetMagicAttackParameters().acceleration.y;

    //音声データの読み込み
    chargeMagicAudioHandle_ = audio_->LoadAudioFile("ChargeFire.mp3");
}

void PlayerStateChargeMagicAttack::Update()
{
    //現在のアニメーションの時間を取得
    float currentAnimationTime = player_->GetIsBlendingCompleted() ? player_->GetCurrentAnimationTime() : player_->GetNextAnimationTime();

    //アニメーションフェーズを更新
    UpdateAnimationPhase(currentAnimationTime);

    //重力加速度を加算して速度を更新
    velocity_ += acceleration_ * GameTimer::GetDeltaTime();

    //プレイヤーを移動
    player_->Move(velocity_);

    //地面に埋まらないようにする
    Vector3 currentPosition = player_->GetPosition();
    if (currentPosition.y <= 0.0f)
    {
        currentPosition.y = 0.0f;
        player_->SetPosition(currentPosition);
    }

    //落下開始時に状態を遷移
    if (animationState_.phases[phaseIndex_].name == "Recovery")
    {
        //地面に足がついていれば通常状態に戻す
        if (currentPosition.y <= 0.0f)
        {
            player_->ChangeState(new PlayerStateRoot());
        }
        //地面に足がついていなければ落下状態にする
        else
        {
            player_->ChangeState(new PlayerStateFalling());
        }
    }
}

void PlayerStateChargeMagicAttack::OnCollision(GameObject* other)
{
    //衝突処理
    player_->ProcessCollisionImpact(other, false);
}

void PlayerStateChargeMagicAttack::SetMagicAttackAnimation(const std::string& animationName)
{
    //指定したアニメーション名に基づいてアニメーションの状態を取得し再生
    animationState_ = player_->GetCombatAnimationEditor()->GetAnimationState(animationName);
    player_->PlayAnimation(animationName, 2.0f, false);
    player_->SetNextAnimationTime(animationState_.phases[phaseIndex_].duration);
}

void PlayerStateChargeMagicAttack::UpdateAnimationPhase(float currentAnimationTime)
{
    //次のフェーズに進むか確認
    if (phaseIndex_ < animationState_.phases.size() - 1 && currentAnimationTime >= animationState_.phases[phaseIndex_].duration)
    {
        //フェーズのインデックスを進める
        phaseIndex_++;

        //現在のフェーズに基づいて速度を更新
        UpdateVelocityForPhase();

        //現在のフェーズが発射フェーズの場合
        if (animationState_.phases[phaseIndex_].name == "Fire")
        {
            //魔法生成
            CreateMagicProjectile();

            //パーティクルを生成
            CreateChargeMagicParticles();

            //音声データを再生
            audio_->PlayAudio(chargeMagicAudioHandle_, false, 0.2f);
        }
    }
}

void PlayerStateChargeMagicAttack::UpdateVelocityForPhase()
{
    //速度をフェーズの移動設定に基づいて更新
    Vector3 currentVelocity = velocity_;
    velocity_ = Mathf::RotateVector({ 0.0f, 0.0f, animationState_.phases[phaseIndex_].attackSettings.moveSpeed }, player_->GetDestinationQuaternion());
    velocity_.y = currentVelocity.y; //垂直方向の速度を保持
}

void PlayerStateChargeMagicAttack::CreateMagicProjectile()
{
    //魔法を生成
    MagicProjectile* magicProjectile = GameObjectManager::CreateGameObject<MagicProjectile>();
    magicProjectile->SetMagicType(MagicProjectile::MagicType::kCharged);
    magicProjectile->SetVelocity(GetMagicProjectileVelocity());
    magicProjectile->SetKnockbackSettings(animationState_.phases[phaseIndex_].knockbackSettings);
    magicProjectile->SetDamage(animationState_.phases[phaseIndex_].attackSettings.damage);

    //魔法の位置と向き設定
    SetMagicProjectileTransform(magicProjectile);
}

const Vector3 PlayerStateChargeMagicAttack::GetMagicProjectileVelocity()
{
    //基本の速度を設定
    Vector3 velocity = { 0.0f, 0.0f, player_->GetMagicAttackParameters().magicProjectileSpeed };

    //ロックオン中の場合、敵の位置に基づいて速度を調整
    if (player_->GetLockon()->ExistTarget())
    {
        Vector3 orbPosition = player_->GetJointWorldPosition("mixamorig:LeftHand");
        Vector3 enemyPosition = GameObjectManager::GetInstance()->GetMutableGameObject<Enemy>("")->GetHipWorldPosition();
        velocity = Mathf::Normalize(enemyPosition - orbPosition) * player_->GetMagicAttackParameters().magicProjectileSpeed;
    }
    //敵がロックオンされていない場合、プレイヤーの方向に基づいて速度を回転
    else
    {
        const Player* player = GameObjectManager::GetInstance()->GetConstGameObject<Player>("");
        velocity = Mathf::RotateVector(velocity, player->GetDestinationQuaternion());
    }

    //計算された速度を返す
    return velocity;
}

void PlayerStateChargeMagicAttack::SetMagicProjectileTransform(MagicProjectile* magicProjectile)
{
    //魔法の位置と向きを設定
    TransformComponent* transformComponent = magicProjectile->GetComponent<TransformComponent>();
    transformComponent->worldTransform_.translation_ = player_->GetJointWorldPosition("mixamorig:LeftHand");
    transformComponent->worldTransform_.quaternion_ = player_->GetDestinationQuaternion();
    transformComponent->Update();
}

void PlayerStateChargeMagicAttack::CreateChargeMagicParticles()
{
    //エミッターの座標を計算
    Vector3 emitterTranslation = (player_->GetJointWorldPosition("mixamorig:LeftHand") + player_->GetJointWorldPosition("mixamorig:RightHand")) / 2.0f;

    //パーティクルを出す
    for (uint32_t i = 0; i < 360; ++i)
    {
        //パーティクルの速度
        const float kParticleVelocity = 0.3f;
        //クォータニオンを計算
        Quaternion rotation = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,0.0f,1.0f }, static_cast<float>(i) * (std::numbers::pi_v<float> / 180.0f));
        //速度を計算
        Vector3 velocity = Mathf::RotateVector({ 0.0f,1.0f,0.0f }, player_->GetDestinationQuaternion() * rotation) * kParticleVelocity;

        //エミッターの生成
        ParticleEmitter* emitter = EmitterBuilder()
            .SetEmitterName("ChargeMagic")
            .SetColor({ 1.0f, 0.4f, 0.4f, 1.0f }, { 1.0f, 0.4f, 0.4f, 1.0f })
            .SetEmitterLifeTime(0.1f)
            .SetCount(1)
            .SetFrequency(0.2f)
            .SetLifeTime(0.3f, 0.3f)
            .SetRadius(0.1f)
            .SetScale({ 0.1f,0.1f,0.1f }, { 0.3f,0.3f,0.3f })
            .SetTranslation(emitterTranslation)
            .SetVelocity(velocity, velocity)
            .Build();
        player_->AddParticleEmitter("Normal", emitter);
    }
}