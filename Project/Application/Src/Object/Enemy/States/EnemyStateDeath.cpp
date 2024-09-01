#include "EnemyStateDeath.h"
#include "Application/Src/Object/Enemy/Enemy.h"

void EnemyStateDeath::Initialize()
{
	//死亡アニメーションを再生
	enemy_->PlayAnimation("Death", 1.0f, false);
}

void EnemyStateDeath::Update()
{
	//ノックバックの処理
	enemy_->ApplyKnockback();

	//敵の座標を取得
	Vector3 position = enemy_->GetPosition();

	//敵が地面についた場合
	if (position.y < 0.0f)
	{
		//プレイヤーが地面に埋まらないように座標を補正
		position.y = 0.0f;
		enemy_->SetPosition(position);
	}
}

void EnemyStateDeath::OnCollision(GameObject* other)
{
}
