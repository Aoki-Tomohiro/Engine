#include "EnemyStateEarthSpikeAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRoot.h"
#include "Application/Src/Object/Player/Player.h"

void EnemyStateEarthSpikeAttack::Initialize()
{
	//地面から突起が出る魔法のアニメーションを再生する
	enemy_->PlayAnimation("EarthSpike", 0.8f, false);

	//アニメーションの状態を取得して設定
	animationState_ = enemy_->GetCombatAnimationEditor()->GetAnimationState("EarthSpike");

	//プレイヤーへの方向ベクトルを計算し、敵を回転させる
	Vector3 directionToPlayer = GetDirectionToPlayer();
	enemy_->Rotate(directionToPlayer);
}

void EnemyStateEarthSpikeAttack::Update()
{
	//現在のアニメーション時間を取得
	float currentAnimationTime = enemy_->GetIsBlendingCompleted() ? enemy_->GetCurrentAnimationTime() : enemy_->GetNextAnimationTime();

	//アニメーションのフェーズを更新
	UpdateAnimationPhase(currentAnimationTime);

	//チャージ状態の時はプレイヤーの方向に回転させる
	if (animationState_.phases[phaseIndex_].name == "Charge")
	{
		Vector3 directionToPlayer = GetDirectionToPlayer();
		enemy_->Rotate(directionToPlayer);
	}

	//アニメーションが終了したら通常状態に戻す
	if (enemy_->GetIsAnimationFinished())
	{
		enemy_->ChangeState(new EnemyStateRoot());
	}
}

void EnemyStateEarthSpikeAttack::OnCollision(GameObject* other)
{
	//衝突処理
	enemy_->ProcessCollisionImpact(other, false);
}

Vector3 EnemyStateEarthSpikeAttack::GetDirectionToPlayer()
{
	//プレイヤーへの方向ベクトルを計算し、y成分を0に設定して正規化
	Vector3 directionToPlayer = GameObjectManager::GetInstance()->GetMutableGameObject<Player>("")->GetHipWorldPosition() - enemy_->GetHipWorldPosition();
	directionToPlayer.y = 0.0f;
	return Mathf::Normalize(directionToPlayer);
}

void EnemyStateEarthSpikeAttack::UpdateAnimationPhase(const float currentAnimationTime)
{
	//現在のアニメーション時間がフェーズの持続時間を超えた場合、次のフェーズに進む
	if (phaseIndex_ < animationState_.phases.size() - 1 && currentAnimationTime > animationState_.phases[phaseIndex_].duration)
	{
		//フェーズを進める
		phaseIndex_++;

		//現在のフェーズが攻撃状態になった場合
		if (animationState_.phases[phaseIndex_].name == "Attack")
		{
			//地面の柱を生成する
			CreatePillar();
		}
	}
}

void EnemyStateEarthSpikeAttack::CreatePillar()
{
	//柱の最大数分生成する
	for (int32_t i = 0; i < enemy_->GetEarthSpikeAttackParameters().maxPillarCount; ++i)
	{
		//柱を生成
		Pillar* newPillar = GameObjectManager::CreateGameObject<Pillar>();

		//非アクティブ時間を設定
		newPillar->SetInactiveDuration(static_cast<float>(i) * enemy_->GetEarthSpikeAttackParameters().nextPillarDelay);

		//柱の生成前の座標を設定
		Vector3 pillarScale = enemy_->GetEarthSpikeAttackParameters().pillarScale;
		Vector3 preSpawnPosition = enemy_->GetPosition() + Mathf::RotateVector(Vector3{ 0.0f, 0.0f, pillarScale.z * 2.0f + static_cast<float>(i)* (pillarScale.z * 2.0f) }, enemy_->GetDestinationQuaternion());
		preSpawnPosition .y = -20.0f;
		newPillar->SetPreSpawnPosition(preSpawnPosition);

		//柱の生成後の座標を設定
		Vector3 spawnedPosition = preSpawnPosition;
		spawnedPosition.y = -10.0f;
		newPillar->SetSpawnedPosition(spawnedPosition);

		//柱のトランスフォームの初期化
		TransformComponent* pillarTransform = newPillar->GetComponent<TransformComponent>();
		pillarTransform->worldTransform_.translation_ = preSpawnPosition;
		pillarTransform->worldTransform_.quaternion_ = enemy_->GetDestinationQuaternion();
		pillarTransform->worldTransform_.scale_ = pillarScale;
	}
}