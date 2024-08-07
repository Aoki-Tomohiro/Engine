#include "EnemyStateLaserAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Collision/OBBCollider.h"
#include "Engine/Components/Collision/CollisionAttributeManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRoot.h"
#include "Application/Src/Object/Enemy/States/EnemyStateDead.h"
#include "Application/Src/Object/Laser/Laser.h"

void EnemyStateLaserAttack::Initialize()
{
	//アニメーションの設定
	ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
	modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.004");
	modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
	modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);

	WorldTransform leftHandWorldTransform = modelComponent->GetModel()->GetAnimation()->GetJointWorldTransform("mixamorig:LeftHand");
	WorldTransform rightHandWorldTransform = modelComponent->GetModel()->GetAnimation()->GetJointWorldTransform("mixamorig:RightHand");
	Vector3 laserPosition = (Vector3{ leftHandWorldTransform.matWorld_.m[3][0],leftHandWorldTransform.matWorld_.m[3][1] ,leftHandWorldTransform.matWorld_.m[3][2] } +
		Vector3{ rightHandWorldTransform.matWorld_.m[3][0],rightHandWorldTransform.matWorld_.m[3][1] ,rightHandWorldTransform.matWorld_.m[3][2] }) / 2.0f;
	Vector3 offset = { 0.0f,0.0f,enemy_->laserAttackParameters_.laserRange + 6.0f };
	laserPosition += Mathf::RotateVector(offset, enemy_->destinationQuaternion_);
	//警告用のオブジェクトの設定
	Warning::BoxWarningObjectSettings warningObjectSettings{};
	warningObjectSettings.warningPrimitive = Warning::kBox;
	warningObjectSettings.position = { laserPosition.x,0.0f,laserPosition.z };
	warningObjectSettings.quaternion = enemy_->destinationQuaternion_;
	warningObjectSettings.scale = { 2.0f,0.01f,enemy_->laserAttackParameters_.laserRange };
	warningObjectSettings.colliderCenter = { 0.0f,laserPosition.y,0.0f };
	warningObjectSettings.colliderSize = { 2.0f,2.0f,enemy_->laserAttackParameters_.laserRange };
	//警告用のオブジェクトを作成
	warning_ = enemy_->CreateBoxWarningObject(warningObjectSettings);

	//ノックバック速度を設定
	enemy_->knockbackSpeed_ = enemy_->laserAttackParameters_.attackParameters.knockbackSpeed;
	//ダメージを設定
	enemy_->damage_ = enemy_->laserAttackParameters_.attackParameters.damage;

	//音声を鳴らす
	enemy_->audio_->PlayAudio(enemy_->chargeAudioHandle_, false, 0.2f);
}

void EnemyStateLaserAttack::Update()
{
	//デバッグのフラグが立っているとき
	ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
	if (enemy_->isDebug_)
	{
		//アニメーションを再生するかを決める
		modelComponent->GetModel()->GetAnimation()->SetIsStop(enemy_->isAnimationStop_);
		//アニメーションが再生されていなければアニメーションの時間を設定
		if (enemy_->isAnimationStop_)
		{
			//アニメーションが再生されていなければアニメーションの時間を設定
			modelComponent->GetModel()->GetAnimation()->SetAnimationTime(enemy_->animationTime_);
		}
		//現在の状態ごとに色を変える
		switch (currentAttackState_)
		{
		case kCharge:
			modelComponent->GetModel()->GetMaterial(0)->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });
			break;
		case kWarning:
			modelComponent->GetModel()->GetMaterial(0)->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
			break;
		case kAttacking:
			modelComponent->GetModel()->GetMaterial(0)->SetColor({ 1.0f, 0.5f, 0.0f, 1.0f });
			break;
		case kRecovery:
			modelComponent->GetModel()->GetMaterial(0)->SetColor({ 0.0f, 0.0f, 1.0f, 1.0f });
			break;
		}
	}
	else
	{
		modelComponent->GetModel()->GetMaterial(0)->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	}

	//前のフレームの状態を更新
	preAttackState_ = currentAttackState_;

	//現在のアニメーションの時間を取得
	float currentAnimationTime = modelComponent->GetModel()->GetAnimation()->GetAnimationTime();

	//現在のアニメーションの時間が溜め時間を超えていなかったらチャージ状態にする
	if (currentAnimationTime <= enemy_->laserAttackParameters_.attackParameters.chargeDuration)
	{
		currentAttackState_ = kCharge;
	}
	//現在のアニメーションの時間が攻撃時間を超えていなかったら攻撃状態にする
	else if (currentAnimationTime <= enemy_->laserAttackParameters_.attackParameters.warningDuration)
	{
		currentAttackState_ = kWarning;
	}
	//現在のアニメーションの時間が溜め時間を超えていたら硬直状態にする
	else if (currentAnimationTime <= enemy_->laserAttackParameters_.attackParameters.attackDuration)
	{
		currentAttackState_ = kAttacking;
	}
	//現在のアニメーションの時間が攻撃時間を超えていたら硬直状態にする
	else if (currentAnimationTime >= enemy_->laserAttackParameters_.attackParameters.attackDuration)
	{
		currentAttackState_ = kRecovery;
	}

	//攻撃状態ごとの処理
	switch (currentAttackState_)
	{
	case kCharge:
		break;
	case kWarning:
		//攻撃警告のフラグを立てる
		isWarning_ = true;

		//前のフレームがチャージ状態の場合
		if (preAttackState_ != currentAttackState_)
		{
			//パーティクルを出す
			enemy_->CreateWarningParticle();
		}
		break;
	case kAttacking:
		//状態が切り替わったとき
		if (preAttackState_ != currentAttackState_)
		{
			//音声を鳴らす
			enemy_->audio_->PlayAudio(enemy_->laserAudioHandle_, false, 0.4f);

			//レーザーを生成する
			Laser* laser = GameObjectManager::GetInstance()->CreateGameObject<Laser>();
			laser->SetTargetScale({ 2.0f,2.0f,enemy_->laserAttackParameters_.laserRange });
			laser->SetLifeTime(enemy_->laserAttackParameters_.attackParameters.attackDuration - enemy_->laserAttackParameters_.attackParameters.chargeDuration);
			laser->SetEasingSpeed(enemy_->laserAttackParameters_.easingSpeed);
			laser->SetDamage(enemy_->laserAttackParameters_.damage);

			//Transformを追加
			TransformComponent* transformComponent = laser->AddComponent<TransformComponent>();
			transformComponent->Initialize();
			transformComponent->worldTransform_.rotationType_ = RotationType::Quaternion;
			WorldTransform leftHandWorldTransform = modelComponent->GetModel()->GetAnimation()->GetJointWorldTransform("mixamorig:LeftHand");
			WorldTransform rightHandWorldTransform = modelComponent->GetModel()->GetAnimation()->GetJointWorldTransform("mixamorig:RightHand");
			Vector3 laserPosition = (Vector3{ leftHandWorldTransform.matWorld_.m[3][0],leftHandWorldTransform.matWorld_.m[3][1] ,leftHandWorldTransform.matWorld_.m[3][2] } +
				Vector3{ rightHandWorldTransform.matWorld_.m[3][0],rightHandWorldTransform.matWorld_.m[3][1] ,rightHandWorldTransform.matWorld_.m[3][2] }) / 2.0f;
			Vector3 offset = { 0.0f,0.0f,enemy_->laserAttackParameters_.laserRange };
			laserPosition += Mathf::RotateVector(offset, enemy_->destinationQuaternion_);
			transformComponent->worldTransform_.translation_ = laserPosition;
			transformComponent->worldTransform_.scale_ = { 0.0f,0.0f,enemy_->laserAttackParameters_.laserRange };
			transformComponent->worldTransform_.quaternion_ = enemy_->destinationQuaternion_;

			//Modelを追加
			ModelComponent* modelComponent = laser->AddComponent<ModelComponent>();
			modelComponent->Initialize("Laser", Opaque);
			modelComponent->GetModel()->GetMaterial(0)->SetEnableLighting(false);
			modelComponent->GetModel()->GetMaterial(0)->SetReceiveShadows(false);
			modelComponent->GetModel()->SetCastShadows(false);
			modelComponent->SetTransformComponent(transformComponent);

			//Colliderを追加
			OBBCollider* collider = laser->AddComponent<OBBCollider>();
			collider->Initialize();
			collider->SetTransformComponent(transformComponent);
			collider->SetCollisionAttribute(CollisionAttributeManager::GetInstance()->GetAttribute("Laser"));
			collider->SetCollisionMask(CollisionAttributeManager::GetInstance()->GetMask("Laser"));

			//フラグの初期化
			isWarning_ = false;
		}
		break;
	case kRecovery:
		//状態が切り替わったとき
		if (preAttackState_ != currentAttackState_)
		{
			warning_->SetIsDestroy(true);
		}
		break;
	}

	//アニメーションが終了していたら通常状態に戻す
	if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
	{
		modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);
		modelComponent->GetModel()->GetMaterial(0)->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		enemy_->ChangeState(new EnemyStateRoot());
	}
	//HPが0になったら
	else if (enemy_->hp_ <= 0.0f)
	{
		//死亡状態にする
		modelComponent->GetModel()->GetMaterial(0)->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		enemy_->ChangeState(new EnemyStateDead());
	}
}

void EnemyStateLaserAttack::Draw(const Camera& camera)
{
}
