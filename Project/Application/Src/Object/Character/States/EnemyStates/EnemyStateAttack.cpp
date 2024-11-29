#include "EnemyStateAttack.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"

void EnemyStateAttack::Initialize()
{
	//攻撃をランダムに決定
	SetRandomAttack();

	//アニメーションを再生
	SetAnimationControllerAndPlayAnimation(animationName_);
}

void EnemyStateAttack::Update()
{
	//アニメーションイベントを実行
	UpdateAnimationState();

	//アニメーションが終了していた場合
	if (character_->GetAnimator()->GetIsAnimationFinished())
	{
		//攻撃状態をリセット
		GetEnemy()->ResetAttackState();
		//状態を遷移
		HandleStateTransition();
	}
}

void EnemyStateAttack::OnCollision(GameObject* other)
{
	//衝突処理
	character_->ProcessCollisionImpact(other, false);
}

void EnemyStateAttack::SetRandomAttack()
{
    //アニメーションの名前が"Attack"の場合にのみ処理を行う
    if (animationName_ != "Attack") return;

    //前回の攻撃と異なるランダムな攻撃タイプを決定
    Enemy::AttackType nextAction;
    do {
        nextAction = static_cast<Enemy::AttackType>(RandomGenerator::GetRandomInt(0, static_cast<int>(Enemy::AttackType::MaxTypes) - 1));
    } while (nextAction == GetEnemy()->GetPreviousAttack());

    //前回の攻撃を設定
    GetEnemy()->SetPreviousAttack(nextAction);

    //攻撃タイプに応じてアニメーションを設定
    switch (nextAction)
    {
    case Enemy::AttackType::VerticalSlash:
        animationName_ = "VerticalSlash";
        break;
    case Enemy::AttackType::ComboSlash:
        animationName_ = "ComboSlash1";
        break;
    case Enemy::AttackType::SpinSlash:
        animationName_ = "SpinSlash";
        break;
    }
}

void EnemyStateAttack::ProcessAttackEvent(const AttackEvent* attackEvent, const int32_t animationEventIndex)
{
    //基底クラスの呼び出し
    ICharacterState::ProcessAttackEvent(attackEvent, animationEventIndex);

    //ジャスト回避可能の時間を進める
    justDodgeTimer_ += GameTimer::GetDeltaTime();

    //ジャスト回避の時間が一定値を超えていたらジャスト回避受付終了
    if (justDodgeTimer_ > GetEnemy()->GetAttackParameters().justDodgeTime)
    {
        GetEnemy()->SetActionFlag(Enemy::ActionFlag::kIsAttacking, false);
    }
}