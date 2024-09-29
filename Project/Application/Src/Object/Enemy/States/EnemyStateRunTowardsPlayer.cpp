#include "EnemyStateRunTowardsPlayer.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateTackle.h"
#include "Application/Src/Object/Enemy/States/EnemyStateJumpAttack.h"
#include "Application/Src/Object/Enemy/States/EnemyStateComboAttack.h"
#include "Application/Src/Object/Player/Player.h"

void EnemyStateRunTowardsPlayer::Initialize()
{
    //走りアニメーションを再生
    enemy_->PlayAnimation("Run", 1.0f, true);
}

void EnemyStateRunTowardsPlayer::Update()
{
    //プレイヤーと敵の座標を取得
    Vector3 playerPosition = GameObjectManager::GetInstance()->GetMutableGameObject<Player>("")->GetHipWorldPosition();
    Vector3 enemyPosition = enemy_->GetHipWorldPosition();

    //プレイヤーへの方向ベクトルを計算
    Vector3 directionToPlayer = CalculateDirectionToPlayer(playerPosition, enemyPosition);

    //速度を計算して移動させる
    Vector3 velocity = Mathf::Normalize(directionToPlayer) * enemy_->GetRunTowardsPlayerParameters().runSpeed;
    enemy_->Move(velocity);

    //敵をプレイヤーの方向に回転させる
    RotateTowardsPlayer(directionToPlayer);

    //プレイヤーに近づいたら攻撃を始める
    if (Mathf::Length(directionToPlayer) < enemy_->GetRootParameters().stopDistance)
    {
        HandleCloseRangeAttack();
    }
}

void EnemyStateRunTowardsPlayer::OnCollision(GameObject* other)
{

}

Vector3 EnemyStateRunTowardsPlayer::CalculateDirectionToPlayer(const Vector3& playerPosition, const Vector3& enemyPosition)
{
    Vector3 directionToPlayer = playerPosition - enemyPosition;
    directionToPlayer.y = 0.0f; //Y軸方向を無視
    return directionToPlayer;
}

void EnemyStateRunTowardsPlayer::RotateTowardsPlayer(const Vector3& directionToPlayer)
{
    //敵をプレイヤーの方向に回転
    enemy_->Rotate(Mathf::Normalize(directionToPlayer));
}

void EnemyStateRunTowardsPlayer::HandleCloseRangeAttack()
{
    Enemy::CloseRangeAttack nextAction;

    //前回と同じ攻撃を避けるため、ランダムに攻撃を選択
    do
    {
        nextAction = static_cast<Enemy::CloseRangeAttack>(RandomGenerator::GetRandomInt(0, static_cast<int>(Enemy::CloseRangeAttack::kMaxCloseRangeAttacks)));
    } while (nextAction == enemy_->GetPreviousCloseRangeAttack());

    PerformCloseRangeAttack(static_cast<int>(nextAction));
}

void EnemyStateRunTowardsPlayer::PerformCloseRangeAttack(const int attack)
{
    //Enemy::CloseRangeAttackにキャスト
    Enemy::CloseRangeAttack nextAttack = static_cast<Enemy::CloseRangeAttack>(attack);

    //現在の攻撃を前回の攻撃として設定
    enemy_->SetPreviousCloseRangeAttack(nextAttack);

    //攻撃タイプに応じた処理を実行
    switch (nextAttack)
    {
    case Enemy::CloseRangeAttack::kTackle:
        //タックル攻撃の処理
        enemy_->ChangeState(new EnemyStateTackle());
        break;
    case Enemy::CloseRangeAttack::kJumpAttack:
        //ジャンプ攻撃の処理
        enemy_->ChangeState(new EnemyStateJumpAttack());
        break;
    case Enemy::CloseRangeAttack::kComboAttack:
        //コンボ攻撃の処理
        enemy_->ChangeState(new EnemyStateComboAttack());
        break;
    }
}