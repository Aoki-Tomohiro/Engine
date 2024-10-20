#include "PlayerStateChargeMagicAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateRoot.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateFalling.h"

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
    //アニメーターを取得
    AnimatorComponent* animator = player_->GetAnimator();

    //現在のアニメーションの時間を取得
    float currentAnimationTime = animator->GetIsBlendingCompleted() ? animator->GetCurrentAnimationTime() : animator->GetNextAnimationTime();

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
    player_->GetAnimator()->PlayAnimation(animationName, 2.0f, false);
    player_->GetAnimator()->SetNextAnimationTime(animationState_.phases[phaseIndex_].duration);
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

            //エミッターの座標を計算
            Vector3 emitterTranslation = (player_->GetJointWorldPosition("mixamorig:LeftHand") + player_->GetJointWorldPosition("mixamorig:RightHand")) / 2.0f;

            //パーティクルを生成
            player_->GetParticleEffectManager()->CreateParticles("ChargeMagic", emitterTranslation, player_->GetDestinationQuaternion());

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
    Magic* magic = GameObjectManager::CreateGameObject<Magic>("Magic");
    magic->SetMagicType(Magic::MagicType::kCharged);
    magic->SetVelocity(GetMagicProjectileVelocity());
    magic->SetKnockbackSettings(animationState_.phases[phaseIndex_].knockbackSettings);
    magic->SetDamage(animationState_.phases[phaseIndex_].attackSettings.damage);
    magic->SetParticleEffectManager(player_->GetParticleEffectManager());

    //魔法の位置と向き設定
    SetMagicProjectileTransform(magic);
}

const Vector3 PlayerStateChargeMagicAttack::GetMagicProjectileVelocity()
{
    //基本の速度を設定
    Vector3 velocity = { 0.0f, 0.0f, player_->GetMagicAttackParameters().magicProjectileSpeed };

    //ロックオン中の場合、敵の位置に基づいて速度を調整
    if (player_->GetLockon()->ExistTarget())
    {
        Vector3 leftHandPosition = player_->GetJointWorldPosition("mixamorig:LeftHand");
        Vector3 enemyPosition = GameObjectManager::GetInstance()->GetGameObject<Enemy>("Enemy")->GetJointWorldPosition("mixamorig:Hips");
        velocity = Mathf::Normalize(enemyPosition - leftHandPosition) * player_->GetMagicAttackParameters().magicProjectileSpeed;
    }
    //敵がロックオンされていない場合、プレイヤーの方向に基づいて速度を回転
    else
    {
        velocity = Mathf::RotateVector(velocity, player_->GetDestinationQuaternion());
    }

    //計算された速度を返す
    return velocity;
}

void PlayerStateChargeMagicAttack::SetMagicProjectileTransform(Magic* magic)
{
    //魔法の位置と向きを設定
    TransformComponent* transformComponent = magic->GetComponent<TransformComponent>();
    transformComponent->worldTransform_.translation_ = player_->GetJointWorldPosition("mixamorig:LeftHand");
    transformComponent->worldTransform_.quaternion_ = player_->GetDestinationQuaternion();
    transformComponent->Update();
}