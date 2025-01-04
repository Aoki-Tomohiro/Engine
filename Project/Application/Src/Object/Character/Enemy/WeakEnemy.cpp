/**
 * @file WeakEnemy.cpp
 * @brief 弱い敵を管理するファイル
 * @author 青木智滉
 * @date
 */

#include "WeakEnemy.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Player/Player.h"

void WeakEnemy::Initialize()
{
	//基底クラスの初期化
	BaseCharacter::Initialize();

	//状態の初期化
	ChangeState("Idle");
}

void WeakEnemy::Update()
{
}

void WeakEnemy::OnCollision(GameObject* gameObject)
{
}

void WeakEnemy::LookAtPlayer()
{
	//敵の腰のジョイントのワールド座標を取得
	Vector3 enemyPosition = GetJointWorldPosition("mixamorig:Hips");

	//プレイヤーの座標を取得
	Vector3 playerPosition = gameObjectManager_->GetGameObject<Player>("Player")->GetJointWorldPosition("mixamorig:Hips");

	//敵の方向へのベクトルを計算
	Vector3 rotateVector = Mathf::Normalize(playerPosition - enemyPosition);

	//Y軸にだけ回転させたいのでY成分を0にする
	rotateVector.y = 0.0f;

	//回転処理
	Rotate(Mathf::Normalize(rotateVector));
}