#include "PlayerStateMagicAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"

void PlayerStateMagicAttack::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

    //アニメーションブレンドを有効にする
    character_->GetAnimator()->SetIsBlending(true);

    //攻撃フラグを立てる
    GetPlayer()->SetActionFlag(Player::ActionFlag::kIsAttacking, true);

    //溜め魔法攻撃を初期化し、必要に応じてアニメーション開始時間を設定
    if (InitializeMagicAttack(GetPlayer()))
    {
        character_->GetAnimator()->SetNextAnimationTime(chargeMagicAttackStartTime_);
    }
}

void PlayerStateMagicAttack::Update()
{
	//アニメーションイベントを実行
	UpdateAnimationState();

    //アニメーションが終了していた場合
    if (character_->GetAnimator()->GetIsAnimationFinished())
    {
        //攻撃のフラグをリセット
        GetPlayer()->SetActionFlag(Player::ActionFlag::kIsAttacking, false);
        //デフォルトの状態に遷移
        HandleStateTransition();
    }
}

void PlayerStateMagicAttack::OnCollision(GameObject* other)
{
	//衝突処理
	character_->ProcessCollisionImpact(other, true);
}

void PlayerStateMagicAttack::InitializeAttackEvent(const AttackEvent* attackEvent, const int32_t animationEventIndex)
{
    //基底クラスの攻撃イベント初期化を呼び出し
    ICharacterState::InitializeAttackEvent(attackEvent, animationEventIndex);

    //魔法オブジェクトを生成
    Magic* magic = CreateMagicObject(attackEvent);

    //魔法の位置と向きを設定
    SetMagicTransform(magic);
}

bool PlayerStateMagicAttack::InitializeMagicAttack(Player* player)
{
    //溜め魔法攻撃のフラグが有効な場合
    if (player->GetActionFlag(Player::ActionFlag::kChargeMagicAttackEnabled))
    {
        //魔法の種類を設定
        magicType_ = Magic::MagicType::kCharged;
        //フラグをリセット
        player->SetActionFlag(Player::ActionFlag::kChargeMagicAttackEnabled, false);
        //溜め魔法攻撃アニメーションを設定
        SetAnimationControllerAndPlayAnimation("MagicAttack");
        //溜め魔法攻撃発生
        return true;
    }
    //通常魔法攻撃のフラグが有効な場合
    else
    {
        //魔法の種類を設定
        magicType_ = Magic::MagicType::kNormal;
        //フラグをリセットし
        player->SetActionFlag(Player::ActionFlag::kMagicAttackEnabled, false);
        //通常魔法アニメーションを設定
        SetAnimationControllerAndPlayAnimation("Casting");
        //溜め魔法攻撃不発生
        return false;
    }
}

Magic* PlayerStateMagicAttack::CreateMagicObject(const AttackEvent* attackEvent)
{
    //魔法オブジェクトを生成
    Magic* magic = GameObjectManager::CreateGameObject<Magic>("Magic");
    //魔法の種類を設定（溜め魔法）
    magic->SetMagicType(magicType_);
    //パーティクルエフェクトエディターを設定
    magic->SetParticleEffectEditor(character_->GetEditorManager()->GetParticleEffectEditor());
    //魔法の初速度を設定
    magic->SetVelocity(CalculateMagicVelocity());
    //魔法のダメージを設定
    magic->SetDamage(attackEvent->attackParameters.damage);
    //魔法のノックバックパラメーターを設定
    magic->SetKnockbackParameters(CreateKnockbackParameters(attackEvent));
    //生成した魔法を返す
    return magic;
}

Vector3 PlayerStateMagicAttack::CalculateMagicVelocity()
{
    //プレイヤーを取得
    Player* player = GetPlayer();

    //基本の速度を設定
    Vector3 velocity = { 0.0f, 0.0f, player->GetMagicAttackParameters().magicProjectileSpeed };

    //ロックオン中の場合、ターゲットに向けて速度を計算
    if (player->GetLockon()->ExistTarget())
    {
        return CalculateLockonVelocity(player);
    }
    //ロックオンしていない場合、プレイヤーの向きに基づいて速度を回転
    return Mathf::RotateVector(velocity, player->GetDestinationQuaternion());
}

Vector3 PlayerStateMagicAttack::CalculateLockonVelocity(Player* player)
{
    //キャラクターの左手の位置を取得
    Vector3 leftHandPosition = character_->GetJointWorldPosition("mixamorig:LeftHand");
    //敵の位置を取得
    Vector3 enemyPosition = GameObjectManager::GetInstance()->GetGameObject<Enemy>("Enemy")->GetJointWorldPosition("mixamorig:Hips");
    //左手から敵に向かう方向を計算して速度を設定
    return Mathf::Normalize(enemyPosition - leftHandPosition) * player->GetMagicAttackParameters().magicProjectileSpeed;
}

KnockbackParameters PlayerStateMagicAttack::CreateKnockbackParameters(const AttackEvent* attackEvent)
{
    //ノックバックのパラメーターをキャラクターの向きに基づいて回転させて設定
    return {
        Mathf::RotateVector(attackEvent->knockbackParameters.velocity, character_->GetDestinationQuaternion()),
        Mathf::RotateVector(attackEvent->knockbackParameters.acceleration, character_->GetDestinationQuaternion()),
        attackEvent->knockbackParameters.reactionType
    };
}

void PlayerStateMagicAttack::SetMagicTransform(Magic* magic)
{
    //魔法の位置と向きを設定
    TransformComponent* transformComponent = magic->GetComponent<TransformComponent>();
    transformComponent->worldTransform_.translation_ = character_->GetJointWorldPosition("mixamorig:LeftHand");
    transformComponent->worldTransform_.quaternion_ = character_->GetDestinationQuaternion();
    transformComponent->Update();
}