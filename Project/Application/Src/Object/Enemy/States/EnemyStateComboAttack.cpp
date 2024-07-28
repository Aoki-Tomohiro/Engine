#include "EnemyStateComboAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRoot.h"
#include "Application/Src/Object/Enemy/States/EnemyStateDead.h"

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
		//前のフレームと状態が変わっていた場合
		if (preAttackState_ != currentAttackState_)
		{
			//攻撃警告のフラグを立てる
			isWarning_ = true;

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

			//ノックバック速度を設定
			enemy_->knockbackSpeed_ = enemy_->comboAttackParameters_.attackParameters[comboIndex_].knockbackSpeed;

			//ダメージを設定
			enemy_->damage_ = enemy_->comboAttackParameters_.attackParameters[comboIndex_].damage;
		}
		break;
	case kAttacking:

		//前のフレームと状態が変わっていた場合
		if (preAttackState_ != currentAttackState_)
		{
			//攻撃フラグを立てる
			isAttack_ = true;

			//音声を鳴らす
			switch (comboIndex_)
			{
			case 0:
				enemy_->audio_->PlayAudio(enemy_->comboAudioHandle1_, false, 0.4f);
				break;
			case 1:
				enemy_->audio_->PlayAudio(enemy_->comboAudioHandle2_, false, 0.4f);
				break;
			}
		}

		//移動させる
		enemyTransformComponent->worldTransform_.translation_ += enemy_->velocity * GameTimer::GetDeltaTime() * enemy_->timeScale_;
		break;
	case kRecovery:

		//前のフレームと状態が変わっていた場合
		if (preAttackState_ != currentAttackState_)
		{
			//現在のコンボが最後のコンボではない場合
			if (comboIndex_ != enemy_->comboAttackParameters_.maxComboIndex - 1)
			{
				comboIndex_++;
			}
			//最後のコンボの時に音を鳴らす
			else
			{
				enemy_->audio_->PlayAudio(enemy_->comboAudioHandle3_, false, 0.6f);
			}

			//フラグの初期化
			isWarning_ = false;
			isAttack_ = false;
		}
		break;
	}

	//アニメーションが終わっていたら通常状態に戻す
	if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
	{
		modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);
		enemy_->ChangeState(new EnemyStateRoot());
	}
	//HPが0を下回ったら
	else if (enemy_->hp_ <= 0.0f)
	{
		//死亡状態にする
		enemy_->ChangeState(new EnemyStateDead());
	}
}

void EnemyStateComboAttack::Draw(const Camera& camera)
{
}