#include "EnemyStateJumpAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRoot.h"
#include "Application/Src/Object/Enemy/States/EnemyStateDead.h"

void EnemyStateJumpAttack::Initialize()
{
	//アニメーションの設定
	ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();
	modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.003");
	modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
	modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);

	//現在のColliderの中心座標を保存
	AABBCollider* collider = enemy_->GetComponent<AABBCollider>();
	currentCenter_ = collider->GetCenter();
	//腰のJointとコライダーの中心のオフセット値を保存
	offset_ = currentCenter_ - enemy_->GetHipLocalPosition();

	//プレイヤーとの距離を計算
	Player* player = GameObjectManager::GetInstance()->GetGameObject<Player>();
	TransformComponent* playerTransformComponent = player->GetComponent<TransformComponent>();
	Vector3 sub = playerTransformComponent->GetWorldPosition() - enemy_->GetHipWorldPosition();
	sub.y = 0.0f;
	float distance = Mathf::Length(sub);
	//プレイヤーとの距離が一定距離以内の場合プレイヤーの位置に攻撃するようにする
	if (distance < enemy_->jumpAttackParameters_.trackingDistance)
	{
		//追尾フラグを設定
		isTracking_ = true;

		//回転させる
		sub = Mathf::Normalize(sub);
		Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, sub));
		float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, sub);
		enemy_->destinationQuaternion_ = Mathf::Normalize(Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot)));

		//開始座標を設定
		startPosition_ = enemy_->GetComponent<TransformComponent>()->GetWorldPosition();

		//目標座標を設定
		targetPosition_ = playerTransformComponent->GetWorldPosition() + Mathf::RotateVector({ 0.0f,0.0f,-8.0f }, enemy_->destinationQuaternion_);
		targetPosition_.y = 0.0f;
	}

	//ノックバック速度を設定
	enemy_->knockbackSpeed_ = enemy_->jumpAttackParameters_.attackParameters.knockbackSpeed;
	//ダメージを設定
	enemy_->damage_ = enemy_->jumpAttackParameters_.attackParameters.damage;
}

void EnemyStateJumpAttack::Update()
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

	//現在のアニメーションに時間に応じて状態を変える
	if (currentAnimationTime <= enemy_->jumpAttackParameters_.attackParameters.chargeDuration)
	{
		currentAttackState_ = kCharge;
	}
	else if (currentAnimationTime <= enemy_->jumpAttackParameters_.attackParameters.warningDuration)
	{
		currentAttackState_ = kWarning;
	}
	else if (currentAnimationTime <= enemy_->jumpAttackParameters_.attackParameters.attackDuration)
	{
		currentAttackState_ = kAttacking;
	}
	else if (currentAnimationTime >= enemy_->jumpAttackParameters_.attackParameters.attackDuration)
	{
		currentAttackState_ = kRecovery;
	}

	//現在の状態の処理
	TransformComponent* transformComponent = enemy_->GetComponent<TransformComponent>();
	switch (currentAttackState_)
	{
	case kCharge:
		//追尾フラグが立っている場合
		if (isTracking_)
		{
			float easingParameter = 1.0f * (currentAnimationTime / enemy_->jumpAttackParameters_.attackParameters.warningDuration);
			transformComponent->worldTransform_.translation_ = startPosition_ + (targetPosition_ - startPosition_) * Mathf::EaseInCubic(easingParameter);
		}

		break;
	case kWarning:
		//追尾フラグが立っている場合
		if (isTracking_)
		{
			float easingParameter = 1.0f * (currentAnimationTime / enemy_->jumpAttackParameters_.attackParameters.warningDuration);
			transformComponent->worldTransform_.translation_ = startPosition_ + (targetPosition_ - startPosition_) * Mathf::EaseInCubic(easingParameter);
		}

		//前のフレームがチャージ状態の場合パーティクルを出す
		if (preAttackState_ != currentAttackState_)
		{
			enemy_->CreateWarningParticle();

			//攻撃警告のフラグを立てる
			isWarning_ = true;
		}
		break;
	case kAttacking:
		//前のフレームがチャージ状態の場合パーティクルを出す
		if (preAttackState_ != currentAttackState_)
		{
			//攻撃フラグを立てる
			isAttack_ = true;
		}
		break;
	case kRecovery:
		//前のフレームがチャージ状態の場合パーティクルを出す
		if (preAttackState_ != currentAttackState_)
		{
			//音声を鳴らす
			enemy_->audio_->PlayAudio(enemy_->jumpAttackAudioHandle_, false, 0.6f);

			//フラグの初期化
			isWarning_ = false;
			isAttack_ = false;
		}
		break;
	}

	//Colliderを取得
	AABBCollider* collider = enemy_->GetComponent<AABBCollider>();
	//ローカルの腰の座標を取得
	Vector3 hipLocalPosition = enemy_->GetHipLocalPosition();
	//敵のワールド座標からずれた分だけ中心座標をずらす
	collider->SetCenter(hipLocalPosition + offset_);

	//アニメーションの再生が終了していたら
	if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
	{
		//アニメーションのループフラグをfalseにする
		modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);
		modelComponent->GetModel()->GetMaterial(0)->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

		//アニメーション後の座標を代入
		Vector3 postAnimationPosition = enemy_->GetHipWorldPosition();
		transformComponent->worldTransform_.translation_ = { postAnimationPosition.x, 0.0f, postAnimationPosition.z };

		//Colliderの中心座標を元に戻す
		collider->SetCenter(currentCenter_);

		//通常状態に遷移
		enemy_->ChangeState(new EnemyStateRoot());
	}
	//HPが0になったら
	else if (enemy_->hp_ <= 0.0f)
	{
		modelComponent->GetModel()->GetMaterial(0)->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		//死亡状態にする
		enemy_->ChangeState(new EnemyStateDead());
	}
}

void EnemyStateJumpAttack::Draw(const Camera& camera)
{

}