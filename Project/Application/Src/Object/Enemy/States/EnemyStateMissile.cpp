#include "EnemyStateMissile.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Components/Collision/SphereCollider.h"
#include "Engine/Components/Collision/CollisionAttributeManager.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRoot.h"

void EnemyStateMissile::Initialize()
{
	//アニメーションの設定
	ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
	modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.002");
	modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
	modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);
}

void EnemyStateMissile::Update()
{
	//デバッグのフラグが立っているとき
	ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
	if (enemy_->isDebug_)
	{
		modelComponent->GetModel()->GetAnimation()->SetAnimationTime(enemy_->animationTime_);
	}

	//現在のアニメーションの時間を取得
	float currentAnimationTime = modelComponent->GetModel()->GetAnimation()->GetAnimationTime();

	//現在のアニメーションの時間が溜め時間を超えていなかったらチャージ状態にする
	if (currentAnimationTime <= enemy_->missileAttackParameters_.chargeDuration_)
	{
		attackState_ = kCharge;
	}
	//現在のアニメーションの時間が攻撃時間を超えていなかったら攻撃状態にする
	else if (currentAnimationTime <= enemy_->missileAttackParameters_.attackDuration_)
	{
		attackState_ = kAttacking;
	}
	//現在のアニメーションの時間が溜め時間を超えていたら硬直状態にする
	else if (currentAnimationTime > enemy_->missileAttackParameters_.attackDuration_)
	{
		attackState_ = kRecovery;
	}

	//攻撃状態ごとの処理
	float totalFiringTime = enemy_->missileAttackParameters_.attackDuration_ - enemy_->missileAttackParameters_.chargeDuration_;
	float fireInterval = totalFiringTime / enemy_->missileAttackParameters_.maxFireCount;
	switch (attackState_)
	{
	case kCharge:
		break;
	case kAttacking:
		fireTimer_ -= GameTimer::GetDeltaTime() * enemy_->timeScale_;
		if (fireTimer_ <= 0.0f)
		{
			//右方向から出るミサイルの生成
			Vector3 velocity[2];
			velocity[0] = { 1.0f,1.0f,0.0f };
			velocity[0] = Mathf::RotateVector(velocity[0], enemy_->destinationQuaternion_);
			CreateMissile(enemy_->missileAttackParameters_.missileParameters[fireCount_], velocity[0]);
			//左方向から出るミサイルの生成
			velocity[1] = { -1.0f,1.0f,0.0f };
			velocity[1] = Mathf::RotateVector(velocity[1], enemy_->destinationQuaternion_);
			CreateMissile(enemy_->missileAttackParameters_.missileParameters[fireCount_], velocity[1]);

			//タイマーをリセット
			fireTimer_ = fireInterval;

			//発射カウントをインクリメント
			fireCount_++;
		}
		break;
	case kRecovery:
		break;
	}

	//アニメーションが終了していたら通常状態に戻す
	if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
	{
		modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);
		enemy_->ChangeState(new EnemyStateRoot());
	}
}

void EnemyStateMissile::Draw(const Camera& camera)
{
}

void EnemyStateMissile::CreateMissile(const Missile::MissileParameters& missileParameters, const Vector3& velocity)
{
	//ミサイルを生成
	Missile* missile = GameObjectManager::GetInstance()->CreateGameObject<Missile>();
	missile->Initialize();
	missile->SetMissileParameters(missileParameters);
	missile->SetVelocity(velocity);

	//敵の手の座標を取得
	ModelComponent* enemyModelComponent = enemy_->GetComponent<ModelComponent>();
	//手のワールドトランスフォームを取得
	WorldTransform handWorldTransform{};
	if (velocity.x == -1.0f)
	{
		handWorldTransform = enemyModelComponent->GetModel()->GetAnimation()->GetJointWorldTransform("mixamorig:LeftHand");
	}
	else
	{
		handWorldTransform = enemyModelComponent->GetModel()->GetAnimation()->GetJointWorldTransform("mixamorig:RightHand");
	}
	//ワールド座標を取得
	Vector3 handWorldPosition = {
		handWorldTransform.matWorld_.m[3][0],
		handWorldTransform.matWorld_.m[3][1],
		handWorldTransform.matWorld_.m[3][2],
	};

	//TransformComponentを追加
	TransformComponent* enemyTransformComponent = enemy_->GetComponent<TransformComponent>();
	TransformComponent* transformComponent = missile->AddComponent<TransformComponent>();
	transformComponent->Initialize();
	transformComponent->worldTransform_.translation_ = handWorldPosition;
	transformComponent->worldTransform_.scale_ = { 0.4f,0.4f,0.4f };

	//ModelComponentを追加
	ModelComponent* modelComponent = missile->AddComponent<ModelComponent>();
	modelComponent->Initialize("Sphere", Transparent);
	modelComponent->SetTransformComponent(transformComponent);
	modelComponent->GetModel()->GetMaterial(0)->SetColor({ 1.0f, 0.2f, 0.2f, 1.0f });

	//Colliderの追加
	SphereCollider* collider = missile->AddComponent<SphereCollider>();
	collider->SetTransformComponent(transformComponent);
	collider->SetRadius(transformComponent->worldTransform_.scale_.x);
	collider->SetCollisionAttribute(CollisionAttributeManager::GetInstance()->GetAttribute("Missile"));
	collider->SetCollisionMask(CollisionAttributeManager::GetInstance()->GetMask("Missile"));

	//一度更新を入れる
	missile->Update();
}