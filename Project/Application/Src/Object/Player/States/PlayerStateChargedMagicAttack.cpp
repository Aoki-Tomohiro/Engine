#include "PlayerStateChargedMagicAttack.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Player/States/PlayerStateAirAttack.h"
#include "Application/Src/Object/MagicProjectile/MagicProjectile.h"

void PlayerStateChargedMagicAttack::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//TransformComponentを取得
	TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
	//ノックバックの速度と減速度を計算
	if (transformComponent->worldTransform_.translation_.y == 0.0f)
	{
		//ノックバック用の速度を計算
		player_->velocity = Mathf::RotateVector({ 0.0f, 0.0f, player_->magicAttackParameters_.chargeMagicKnockbackSpeed }, player_->destinationQuaternion_);
		decelerationVector_ = { 0.0f,0.0f,player_->magicAttackParameters_.chargeMagicKnockbackDeceleration };
	}
	else
	{
		isInAir_ = true;
		player_->velocity = Mathf::RotateVector({ 0.0f, player_->magicAttackParameters_.chargeMagicVerticalBoost, player_->magicAttackParameters_.chargeMagicKnockbackSpeed }, player_->destinationQuaternion_);
		decelerationVector_ = { 0.0f,player_->magicAttackParameters_.chargeMagicVerticalAcceleration,player_->magicAttackParameters_.chargeMagicKnockbackDeceleration };
	}
	decelerationVector_ = Mathf::RotateVector(decelerationVector_, player_->destinationQuaternion_);

	//アニメーションの変更
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.029");
	modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);
	modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(2.0f);
	modelComponent->GetModel()->GetAnimation()->SetAnimationTime(1.13f);

	//魔法攻撃を出す
	player_->AddMagicProjectile(MagicProjectile::MagicType::kCharged);
}

void PlayerStateChargedMagicAttack::Update()
{
	//アニメーションが終了したら
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	float currentAnimationTime = modelComponent->GetModel()->GetAnimation()->GetAnimationTime();
	if (currentAnimationTime > 1.33f)
	{
		//速度を減速させる
		player_->velocity += decelerationVector_;

		//ノックバックさせる
		TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
		transformComponent->worldTransform_.translation_ += player_->velocity * GameTimer::GetDeltaTime();

		//地面にいる場合
		if (!isInAir_)
		{
			//速度が0に近づいたら
			if (Mathf::Length(player_->velocity) < 1.0f)
			{
				//アニメーションの設定を戻す
				modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);
				modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(1.0f);

				//チャージ魔法攻撃のフラグをリセット
				player_->chargeMagicAttackWork_.isChargeMagicAttack_ = false;

				//通常状態に戻す
				player_->ChangeState(new PlayerStateRoot());
			}
		}
		else
		{
			//プレイヤーが地面についたら
			if (transformComponent->worldTransform_.translation_.y <= 0.0f)
			{
				//座標を固定
				transformComponent->worldTransform_.translation_.y = 0.0f;

				//アニメーションの設定を戻す
				modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);
				modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(1.0f);

				//チャージ魔法攻撃のフラグをリセット
				player_->chargeMagicAttackWork_.isChargeMagicAttack_ = false;

				//通常状態に戻す
				player_->ChangeState(new PlayerStateRoot());
			}
			//攻撃状態に遷移
			else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
			{
				//速度の初期化
				player_->velocity.y = 0.0f;

				//アニメーションの設定を戻す
				modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);
				modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(1.0f);

				//チャージ魔法攻撃のフラグをリセット
				player_->chargeMagicAttackWork_.isChargeMagicAttack_ = false;

				//空中攻撃状態に遷移
				player_->ChangeState(new PlayerStateAirAttack());
			}
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