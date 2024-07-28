#include "PlayerStateKnockback.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Laser/Laser.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Player/States/PlayerStateDead.h"

void PlayerStateKnockback::Initialize()
{
	//アニメーションの初期化
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);
	modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(1.2f);
	modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
	modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.030");

	//速度を設定
	Enemy* enemy = GameObjectManager::GetInstance()->GetGameObject<Enemy>();
	if (player_->lockOn_->ExistTarget())
	{
		player_->velocity = Mathf::RotateVector({ 0.0f,0.0f,enemy->GetKnockbackSpeed() }, player_->destinationQuaternion_);
	}
	else
	{
		TransformComponent* transformComponent = enemy->GetComponent<TransformComponent>();
		player_->velocity = Mathf::RotateVector({ 0.0f,0.0f,enemy->GetKnockbackSpeed() * -1.0f }, transformComponent->worldTransform_.quaternion_);
	}

	//ダメージエフェクトのスプライトの色を変更
	player_->damagedSpriteColor_.w = 0.2f;

	//音声を鳴らす
	player_->audio_->PlayAudio(player_->damageAudioHandle_, false, 0.4f);
}

void PlayerStateKnockback::Update()
{
	//死亡状態に遷移
	if (player_->hp_ <= 0.0f)
	{
		player_->damagedSpriteColor_.w = 0.0f;
		player_->ChangeState(new PlayerStateDead());
		return;
	}

	//ノックバックのフラグを立てる
	player_->isKnockback_ = true;

	//現在のアニメーションの時間を取得
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	float currentAnimationTime = modelComponent->GetModel()->GetAnimation()->GetAnimationTime();

	//プレイヤーがのけぞり状態の時に移動させる
	if (currentAnimationTime < player_->knockBackParameters_.staggerDuration)
	{
		//移動処理
		TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
		transformComponent->worldTransform_.translation_ += player_->velocity * GameTimer::GetDeltaTime();
	}

	//ダメージエフェクトのスプライトの色を徐々に薄くする
	player_->damagedSpriteColor_.w = 0.2f - (0.2f * (currentAnimationTime / modelComponent->GetModel()->GetAnimation()->GetAnimationDuration("Armature.001|mixamo.com|Layer0.030")));

	//アニメーションが終了したら通常状態に戻す
	if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
	{
		//Knockbackのフラグをリセット
		player_->isKnockback_ = false;
		player_->damagedSpriteColor_.w = 0.0f;
		modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);
		modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(1.0f);
		player_->ChangeState(new PlayerStateRoot());
	}
}

void PlayerStateKnockback::Draw(const Camera& camera)
{
}

void PlayerStateKnockback::OnCollision(GameObject* other)
{
}

void PlayerStateKnockback::OnCollisionEnter(GameObject* other)
{
}

void PlayerStateKnockback::OnCollisionExit(GameObject* other)
{
}