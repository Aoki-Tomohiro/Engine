#include "PlayerStateChargedMagicAttack.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"

void PlayerStateChargedMagicAttack::Initialize()
{
	//ノックバック用の速度を計算
	player_->velocity = Mathf::RotateVector({ 0.0f, 0.0f, player_->magicAttackParameters_.chargeMagicKnockbackSpeed }, player_->destinationQuaternion_);
	if (player_->isAirAttack_)
	{
		player_->velocity = Mathf::RotateVector({ 0.0f, player_->magicAttackParameters_.airChargeMagicVerticalBoost, player_->magicAttackParameters_.chargeMagicKnockbackSpeed }, player_->destinationQuaternion_);
	}

	//ノックバックの減速度を計算
	decelerationVector_ = { 0.0f,0.0f,player_->magicAttackParameters_.chargeMagicKnockbackDeceleration };
	decelerationVector_ = Mathf::RotateVector(decelerationVector_, player_->destinationQuaternion_);

	//アニメーションの変更
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.029");
	modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);
	modelComponent->GetModel()->GetAnimation()->SetAnimationTime(1.13f);
}

void PlayerStateChargedMagicAttack::Update()
{
	//アニメーションが終了したら
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
	{
		//速度を減速させる
		player_->velocity += decelerationVector_;

		//ノックバックさせる
		TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
		transformComponent->worldTransform_.translation_ += player_->velocity * GameTimer::GetDeltaTime();

		//速度が0に近づいたら
		if (Mathf::Length(player_->velocity) < 1.0f)
		{
			//アニメーションの設定を戻す
			modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);

			//チャージ魔法攻撃のフラグをリセット
			player_->isChargeMagicAttack_ = false;

			//通常状態に戻す
			player_->ChangeState(new PlayerStateRoot());
		}
	}
}

void PlayerStateChargedMagicAttack::Draw(const Camera& camera)
{
}

void PlayerStateChargedMagicAttack::OnCollision(GameObject* other)
{
}

void PlayerStateChargedMagicAttack::OnCollisionEnter(GameObject* other)
{
}

void PlayerStateChargedMagicAttack::OnCollisionExit(GameObject* other)
{
}