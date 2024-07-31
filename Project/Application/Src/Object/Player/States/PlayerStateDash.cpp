#include "PlayerStateDash.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Laser/Laser.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Player/States/PlayerStateKnockback.h"
#include "Application/Src/Object/Player/States/PlayerStateGroundAttack.h"
#include "Application/Src/Object/Player/States/PlayerStateDead.h"

void PlayerStateDash::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//ロックオン中なら
	if (player_->lockOn_->ExistTarget())
	{
		//敵の座標を取得
		Vector3 enemyPosition = GameObjectManager::GetInstance()->GetGameObject<Enemy>()->GetHipWorldPosition();

		//プレイヤーの座標を取得
		Vector3 playerPosition = player_->GetComponent<TransformComponent>()->worldTransform_.translation_;

		//差分ベクトルを計算
		Vector3 sub = enemyPosition - playerPosition;
		sub.y = 0.0f;

		//速度を計算
		player_->velocity = Mathf::Normalize(sub) * player_->dashParameters_.dashSpeed_;

		//差分ベクトルのY成分を0にする
		sub.y = 0.0f;

		//差分ベクトルを正規化
		sub = Mathf::Normalize(sub);

		//回転処理
		Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, sub));
		float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, sub);
		player_->destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
	}
	else
	{
		//速度ベクトルにプレイヤーの回転を適用
		TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
		player_->velocity = Mathf::TransformNormal({ 0.0f,0.0f,1.0f }, transformComponent->worldTransform_.matWorld_);

		//速度を掛ける
		player_->velocity = Mathf::Normalize(player_->velocity) * player_->dashParameters_.dashSpeed_;
	}

	//RadialBlurをかける
	PostEffects::GetInstance()->GetRadialBlur()->SetIsEnable(true);

	//パーティクルの初期化
	particleSystem_ = ParticleManager::Create("Dash");
}

void PlayerStateDash::Update()
{
	//死亡状態に遷移
	if (player_->hp_ <= 0.0f)
	{
		player_->ChangeState(new PlayerStateDead());
		return;
	}

	//チャージが終わっていない場合
	if (!dashWork_.isChargingFinished)
	{
		//チャージタイマーを進める
		dashWork_.chargeTimer += GameTimer::GetDeltaTime();

		//チャージタイマーが一定値を超えた場合
		if (dashWork_.chargeTimer > player_->dashParameters_.chargeDuration)
		{
			//音声を鳴らす
			player_->audio_->PlayAudio(player_->dashAudioHandle_, false, 0.4f);

			//チャージ終了フラグを立てる
			dashWork_.isChargingFinished = true;

			//プレイヤーを消す
			player_->SetIsVisible(false);

			//武器を消す
			Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>();
			weapon->SetIsVisible(false);

			//パーティクルを出す
			TransformComponent* playerTransformComponent = player_->GetComponent<TransformComponent>();
			Vector3 minVelocity = { 0.0f,0.0f,-0.4f };
			minVelocity = Mathf::TransformNormal(minVelocity, playerTransformComponent->worldTransform_.matWorld_);
			Vector3 maxVelocity = { 0.0f,0.0f,-0.6f };
			maxVelocity = Mathf::TransformNormal(maxVelocity, playerTransformComponent->worldTransform_.matWorld_);
			ParticleEmitter* newEmitter = new ParticleEmitter();
			newEmitter->Initialize("Dash", 100.0f);
			newEmitter->SetTranslate(playerTransformComponent->GetWorldPosition());
			newEmitter->SetCount(40);
			newEmitter->SetColorMin({ 1.0f, 0.2f, 0.2f, 1.0f });
			newEmitter->SetColorMax({ 1.0f, 0.2f, 0.2f, 1.0f });
			newEmitter->SetFrequency(0.001f);
			newEmitter->SetLifeTimeMin(0.2f);
			newEmitter->SetLifeTimeMax(0.4f);
			newEmitter->SetRadius(1.0f);
			newEmitter->SetScaleMin({ 0.2f,0.2f,0.2f });
			newEmitter->SetScaleMax({ 0.3f,0.3f,0.3f });
			newEmitter->SetVelocityMin(minVelocity);
			newEmitter->SetVelocityMax(maxVelocity);
			particleSystem_->AddParticleEmitter(newEmitter);
		}
	}

	//チャージが終わっていてダッシュが終了していない場合
	if (dashWork_.isChargingFinished && !dashWork_.isDashFinished)
	{
		//Xボタンを押した場合攻撃状態に派生
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
		{
			player_->isDashAttack_ = true;
		}

		//ダッシュタイマーを進める
		dashWork_.dashTimer += GameTimer::GetDeltaTime();

		//ダッシュタイマーが一定値を超えていたら
		if (dashWork_.dashTimer > player_->dashParameters_.dashDuration)
		{
			//ダッシュ終了フラグを立てる
			dashWork_.isDashFinished = true;

			//プレイヤーを描画させる
			player_->SetIsVisible(true);

			//武器を描画させる
			Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>();
			weapon->SetIsVisible(true);

			//エミッターを削除
			if (ParticleEmitter* emitter = particleSystem_->GetParticleEmitter("Dash"))
			{
				emitter->SetIsDead(true);
			}

			//アニメーションを変更
			ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
			modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.018");
			modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
			modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);

			//RadialBlurを切る
			PostEffects::GetInstance()->GetRadialBlur()->SetIsEnable(false);
		}

		//速度を加算
		TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
		transformComponent->worldTransform_.translation_ += player_->velocity * GameTimer::GetDeltaTime();

		//敵のTransformを取得
		Vector3 enemyPosition = GameObjectManager::GetInstance()->GetGameObject<Enemy>()->GetHipWorldPosition();

		//敵と一定距離近づいたら速度を0にする
		if (Mathf::Length(enemyPosition - transformComponent->GetWorldPosition()) < player_->dashParameters_.proximityThreshold)
		{
			player_->velocity = { 0.0f,0.0f,0.0f };
		}

		//エミッターの座標を移動させる
		if (ParticleEmitter* emitter = particleSystem_->GetParticleEmitter("Dash"))
		{
			emitter->SetTranslate(transformComponent->worldTransform_.translation_);
		}
	}

	//ダッシュが終わっていたら
	if (dashWork_.isDashFinished)
	{
		//アニメーションの設定を戻す
		ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
		modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);

		//アニメーションが終了したら通常状態に戻す
		if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
		{
			player_->ChangeState(new PlayerStateRoot());
		}
		//ダッシュ攻撃が有効なら攻撃状態に派生
		else if (player_->isDashAttack_)
		{
			player_->ChangeState(new PlayerStateGroundAttack());
		}
	}
}

void PlayerStateDash::Draw(const Camera& camera)
{
}

void PlayerStateDash::OnCollision(GameObject* other)
{
	//パリィ状態でなければ
	if (!player_->isParrying_)
	{
		//衝突相手が敵だった場合
		if (Enemy* enemy = dynamic_cast<Enemy*>(other))
		{
			if (enemy->GetIsAttack())
			{
				player_->isDashAttack_ = false;
				player_->hp_ -= enemy->GetDamage();
				player_->isDamaged_ = true;
				player_->ChangeState(new PlayerStateKnockback());
			}
		}
		//衝突相手がレーザーだった場合
		else if (Laser* laser = dynamic_cast<Laser*>(other))
		{
			player_->isDashAttack_ = false;
			player_->hp_ -= laser->GetDamage();
			player_->isDamaged_ = true;
			player_->ChangeState(new PlayerStateKnockback());
		}
	}
}

void PlayerStateDash::OnCollisionEnter(GameObject* other)
{
}

void PlayerStateDash::OnCollisionExit(GameObject* other)
{
}
