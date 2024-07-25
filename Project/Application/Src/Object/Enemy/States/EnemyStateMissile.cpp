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
}

void EnemyStateMissile::Update()
{
	//アニメーションの速度の更新
	ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
	modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(enemy_->timeScale_);

	//チャージが終了していないとき
	if (!isChargeFinished_)
	{
		//チャージタイマーを進める
		chargeTimer_ += GameTimer::GetDeltaTime() * enemy_->timeScale_;

		//チャージタイマーが一定値を超えていたら
		if (chargeTimer_ > enemy_->missileAttackParameters_.chargeDuration_)
		{
			//ミサイルを発射可能にする
			isFireEnabled_ = true;
		}
	}

	//ミサイルが発射可能の時
	if (isFireEnabled_)
	{
		//発射カウントが一定値を超えていなかったら
		if (fireCount_ < enemy_->missileAttackParameters_.missileParameters.size())
		{
			//発射タイマーを進める
			fireTimer_ += GameTimer::GetDeltaTime() * enemy_->timeScale_;

			//発射タイマーが一定間隔を超えていたら
			if (fireTimer_ > enemy_->missileAttackParameters_.fireInterval)
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

				//ミサイルの発射カウントをインクリメント
				fireCount_++;

				//タイマーをリセット
				fireTimer_ = 0.0f;
			}
		}
		else
		{
			isRecovery_ = true;
		}
	}

	//硬直のフラグが立っていたら
	if (isRecovery_)
	{
		//硬直タイマーを進める
		recoveryTimer_ += GameTimer::GetDeltaTime() * enemy_->timeScale_;

		//硬直タイマーが一定値を超えていたら通常状態に戻す
		if (recoveryTimer_ > enemy_->missileAttackParameters_.recoveryDuration_)
		{
			enemy_->ChangeState(new EnemyStateRoot());
		}
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

	//TransformComponentを追加
	TransformComponent* enemyTransformComponent = enemy_->GetComponent<TransformComponent>();
	TransformComponent* transformComponent = missile->AddComponent<TransformComponent>();
	transformComponent->Initialize();
	transformComponent->worldTransform_.translation_ = enemyTransformComponent->GetWorldPosition() + Vector3{0.0f, 5.0f, 0.0f};
	transformComponent->worldTransform_.scale_ = { 0.4f,0.4f,0.4f };

	//ModelComponentを追加
	ModelComponent* modelComponent = missile->AddComponent<ModelComponent>();
	modelComponent->Initialize("Fire", Transparent);
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