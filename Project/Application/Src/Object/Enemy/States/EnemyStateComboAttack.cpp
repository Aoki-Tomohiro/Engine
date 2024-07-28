#include "EnemyStateComboAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRoot.h"

void EnemyStateComboAttack::Initialize()
{
	//アニメーションの設定
	ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
	modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.002");
	modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
	modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);
}

void EnemyStateComboAttack::Update()
{
	//デバッグのフラグが立っているとき
	ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
	if (enemy_->isDebug_)
	{
		modelComponent->GetModel()->GetAnimation()->SetAnimationTime(enemy_->animationTime_);
	}

	//前のフレームの状態を更新
	preAttackState_ = currentAttackState_;

	//現在のアニメーションの時間を取得
	float currentAnimationTime = modelComponent->GetModel()->GetAnimation()->GetAnimationTime();
	//現在のアニメーションに時間に応じて状態を変える
	if (currentAnimationTime <= enemy_->comboAttackParameters_.attackParameters[comboIndex_].chargeDuration)
	{
		currentAttackState_ = kCharge;
	}
	else if (currentAnimationTime <= enemy_->comboAttackParameters_.attackParameters[comboIndex_].warningDuration)
	{
		currentAttackState_ = kWarning;
	}
	else if (currentAnimationTime <= enemy_->comboAttackParameters_.attackParameters[comboIndex_].attackDuration)
	{
		currentAttackState_ = kAttacking;
	}
	else if (currentAnimationTime >= enemy_->comboAttackParameters_.attackParameters[comboIndex_].attackDuration)
	{
		currentAttackState_ = kRecovery;
	}

	//プレイヤーのトランスフォームを取得
	TransformComponent* playerTransformComponent = GameObjectManager::GetInstance()->GetGameObject<Player>()->GetComponent<TransformComponent>();
	//自分のトランスフォームを取得
	TransformComponent* enemyTransformComponent = enemy_->GetComponent<TransformComponent>();
	//腰の座標を取得
	Vector3 enemyHipPosition = enemy_->GetHipWorldPosition();
	//差分ベクトルを計算
	Vector3 sub = playerTransformComponent->GetWorldPosition() - enemyHipPosition;
	sub.y = 0.0f;
	//距離を計算
	float distance = Mathf::Length(sub);

	//現在の状態の処理
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
			//プレイヤーとの距離が近ければ
			if (distance <= enemy_->comboAttackParameters_.moveDistance)
			{
				//回転させる
				sub = Mathf::Normalize(sub);
				Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, sub));
				float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, sub);
				enemy_->destinationQuaternion_ = Mathf::Normalize(Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot)));
			}

			//パーティクルを出す
			enemy_->CreateWarningParticle();

			//速度を計算
			enemy_->velocity = Mathf::RotateVector(enemy_->comboAttackParameters_.attackVelocity[comboIndex_], enemy_->destinationQuaternion_);
		}
		break;
	case kAttacking:

		//攻撃フラグを立てる
		isAttack_ = true;

		//移動させる
		enemyTransformComponent->worldTransform_.translation_ += enemy_->velocity * GameTimer::GetDeltaTime() * enemy_->timeScale_;
		break;
	case kRecovery:

		//フラグの初期化
		isWarning_ = false;
		isAttack_ = false;

		//状態が切り替わったとき
		if (preAttackState_ != currentAttackState_)
		{
			//現在のコンボが最後のコンボではない場合
			if (comboIndex_ != enemy_->comboAttackParameters_.maxComboIndex - 1)
			{
				comboIndex_++;
			}
		}
		break;
	}

	//アニメーションが終わっていたら通常状態に戻す
	if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
	{
		modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);
		enemy_->ChangeState(new EnemyStateRoot());
	}
}

void EnemyStateComboAttack::Draw(const Camera& camera)
{
}