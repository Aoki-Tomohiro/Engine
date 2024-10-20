#include "PlayerStateMagicAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateRoot.h"

void PlayerStateMagicAttack::Initialize()
{
    //オーディオのインスタンスを取得
    audio_ = Audio::GetInstance();

    //インプットのインスタンスを取得
    input_ = Input::GetInstance();

    //アニメーション設定と再生
    SetMagicAttackAnimation("Casting");

    //魔法攻撃フラグのリセット
    player_->SetActionFlag(Player::ActionFlag::kMagicAttackEnabled, false);

    //クールダウンのリセット
    player_->ResetMagicAttackCooldownTime();

    //音声データの読み込み
    fireAudioHandle_ = audio_->LoadAudioFile("Fire.mp3");
}

void PlayerStateMagicAttack::Update()
{
    //アニメーターを取得
    AnimatorComponent* animator = player_->GetAnimator();

    //現在のアニメーションの時間を取得
    float currentAnimationTime = animator->GetIsBlendingCompleted() ? animator->GetCurrentAnimationTime() : animator->GetNextAnimationTime();

    //アニメーションフェーズを更新
    UpdateAnimationPhase(currentAnimationTime);

    //魔法攻撃処理
    ExecuteMagicAttack();

    //プレイヤーの回転を更新
    UpdateRotation();

    //アニメーションが終了した場合、または硬直キャンセル状態の場合に状態を変更
    if (animator->GetIsAnimationFinished() || CheckRecoveryCancel())
    {
        player_->ChangeState(new PlayerStateRoot());
    }
}

void PlayerStateMagicAttack::OnCollision(GameObject* other)
{
    //衝突処理
    player_->ProcessCollisionImpact(other, true);
}

void PlayerStateMagicAttack::SetMagicAttackAnimation(const std::string& animationName)
{
    //指定したアニメーション名に基づいてアニメーションの状態を取得し再生
    animationState_ = player_->GetCombatAnimationEditor()->GetAnimationState(animationName);
    player_->GetAnimator()->PlayAnimation(animationName, 1.0f, false);
    player_->GetAnimator()->SetNextAnimationTime(animationState_.phases[phaseIndex_].duration);
}

void PlayerStateMagicAttack::UpdateAnimationPhase(float currentAnimationTime)
{
    //次のフェーズに進むか確認
    if (phaseIndex_ < animationState_.phases.size() - 1 && currentAnimationTime >= animationState_.phases[phaseIndex_].duration)
    {
        //フェーズのインデックスを進める
        phaseIndex_++;

        //現在のフェーズが発射フェーズの場合
        if (animationState_.phases[phaseIndex_].name == "Fire")
        {
            //魔法生成
            CreateMagicProjectile();
        }
    }
}

void PlayerStateMagicAttack::UpdateRotation()
{
    //ロックオン中の場合プレイヤーを回転させる
    if (player_->GetLockon()->ExistTarget())
    {
        player_->LookAtEnemy();
    }
}

void PlayerStateMagicAttack::ExecuteMagicAttack()
{
    //Yボタンを離した時かつ魔法攻撃が有効な場合魔法を生成
    if (input_->IsPressButtonExit(XINPUT_GAMEPAD_Y) && player_->GetActionFlag(Player::ActionFlag::kMagicAttackEnabled))
    {
        ResetToFirePhase();
        CreateMagicProjectile();
    }
}

void PlayerStateMagicAttack::ResetToFirePhase()
{
    //フェーズインデックスをリセットする
    phaseIndex_ = kFire;
    //アニメーションの時間を設定
    player_->GetAnimator()->SetCurrentAnimationTime(animationState_.phases[0].duration);
    //魔法攻撃フラグをリセット
    player_->SetActionFlag(Player::ActionFlag::kMagicAttackEnabled, false);
    //クールダウンをリセット
    player_->ResetMagicAttackCooldownTime();
}

void PlayerStateMagicAttack::CreateMagicProjectile()
{
    //魔法を生成
    Magic* magic = GameObjectManager::CreateGameObject<Magic>("Magic");
    magic->SetMagicType(Magic::MagicType::kNormal);
    magic->SetVelocity(GetMagicProjectileVelocity());
    magic->SetKnockbackSettings(animationState_.phases[phaseIndex_].knockbackSettings);
    magic->SetDamage(animationState_.phases[phaseIndex_].attackSettings.damage);
    magic->SetParticleEffectManager(player_->GetParticleEffectManager());

    //魔法の位置と向き設定
    SetMagicProjectileTransform(magic);

    //音声データを再生
    audio_->PlayAudio(fireAudioHandle_, false, 0.2f);
}

const Vector3 PlayerStateMagicAttack::GetMagicProjectileVelocity()
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

void PlayerStateMagicAttack::SetMagicProjectileTransform(Magic* magic)
{
    //魔法の位置と向きを設定
    TransformComponent* transformComponent = magic->GetComponent<TransformComponent>();
    transformComponent->worldTransform_.translation_ = player_->GetJointWorldPosition("mixamorig:LeftHand");
    transformComponent->worldTransform_.quaternion_ = player_->GetDestinationQuaternion();
    transformComponent->Update();
}

bool PlayerStateMagicAttack::CheckRecoveryCancel()
{
    //硬直中の場合
    if (animationState_.phases[phaseIndex_].name == "Recovery")
    {
        //左スティックの入力値を取得
        Vector3 inputValue = {
            input_->GetLeftStickX(),
            0.0f,
            input_->GetLeftStickY(),
        };

        //入力が歩きの閾値を超えている場合、硬直キャンセル
        if (Mathf::Length(inputValue) > player_->GetRootParameters().walkThreshold)
        {
            return true;
        }
    }
    return false;
}