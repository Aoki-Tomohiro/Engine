#include "PlayerStateDash.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Enemy/Enemy.h"

void PlayerStateDash::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//ダッシュのフラグを設定
	player_->SetActionFlag(Player::ActionFlag::kDashing, true);

	//アニメーションの初期化
	player_->SetAnimationName("Armature.001|mixamo.com|Layer0.014");
	player_->SetAnimationTime(0.0f);
	player_->SetIsAnimationStop(true);

	//ロックオン中なら
	if (player_->GetLockon()->ExistTarget())
	{
		//敵の座標を取得
		Vector3 enemyPosition = GameObjectManager::GetInstance()->GetConstGameObject<Enemy>("")->GetHipWorldPosition();

		//プレイヤーの座標を取得
		Vector3 playerPosition = player_->GetHipWorldPosition();

		//差分ベクトルを計算
		Vector3 sub = Mathf::Normalize(enemyPosition - playerPosition);

		//水平方向に移動させるので速度のY成分を0にする
		sub.y = 0.0f;

		//速度を計算
		velocity_ = sub * player_->GetDashParameters().dashSpeed;

		//回転処理
		player_->Rotate(sub);
	}
	//ロックオン中ではない場合
	else
	{
		//速度を計算
		velocity_ = Mathf::RotateVector({ 0.0f,0.0f,-1.0f }, player_->GetDestinationQuaternion());

		//速度を掛ける
		velocity_ = Mathf::Normalize(velocity_) * player_->GetDashParameters().dashSpeed;
	}

	//状態の取得
	state_ = player_->GetAnimationStateManager()->GetAnimationState("Dash");

	//最初のフェーズの名前を設定
	phaseName_ = state_.phases[0].name;

	//前のフレームのフェーズ名の初期化
	prePhaseName_ = phaseName_;

	//RadialBlurを掛ける
	PostEffects::GetInstance()->GetRadialBlur()->SetIsEnable(true);

	//テクスチャの読み込み
	TextureManager::Load("smoke_01.png");

	//パーティクルの作成
	particleSystem_ = ParticleManager::Create("Dash");
	particleSystem_->SetTexture("smoke_01.png");

	//加速フィールドを作成
	AccelerationField* accelerationField = new AccelerationField();
	accelerationField->Initialize("Dash", 100.0f);
	accelerationField->SetAcceleration({ 0.0f,0.8f,0.0f });
	accelerationField->SetMin({ -200.0f,-200.0f,-200.0f });
	accelerationField->SetMax({ 200.0f,200.0f,200.0f });
	particleSystem_->AddAccelerationField(accelerationField);
}

void PlayerStateDash::Update()
{
	//前のフレームのフェーズ名を代入
	prePhaseName_ = phaseName_;

	//アニメーションの時間を進める
	animationTime_ += GameTimer::GetDeltaTime();

	//状態の更新
	for (uint32_t i = 0; i < state_.phases.size() - 1; ++i)
	{
		if (phaseName_ == state_.phases[i].name)
		{
			if (animationTime_ >= state_.phases[i].animationTime)
			{
				phaseName_ = state_.phases[i + 1].name;
			}
		}
	}

	//状態ごとの処理
	if (phaseName_ == "Dash")
	{
		//フェーズが変わったとき
		if (prePhaseName_ != phaseName_)
		{
			//プレイヤーを消す
			player_->SetIsVisible(false);

			//パーティクルを出す
			Vector3 minVelocity = Mathf::RotateVector({ 0.0f, 0.0f, 0.4f }, player_->GetDestinationQuaternion());
			Vector3 maxVelocity = Mathf::RotateVector({ 0.0f, 0.0f, 0.6f }, player_->GetDestinationQuaternion());
			ParticleEmitter* newEmitter = new ParticleEmitter();
			newEmitter->Initialize("Dash", 100.0f);
			newEmitter->SetTranslate(player_->GetHipWorldPosition());
			newEmitter->SetCount(40);
			newEmitter->SetColorMin({ 1.0f, 0.2f, 0.2f, 1.0f });
			newEmitter->SetColorMax({ 1.0f, 0.2f, 0.2f, 1.0f });
			newEmitter->SetFrequency(0.001f);
			newEmitter->SetLifeTimeMin(0.4f);
			newEmitter->SetLifeTimeMax(0.8f);
			newEmitter->SetRadius(1.0f);
			newEmitter->SetScaleMin({ 0.8f,0.8f,0.8f });
			newEmitter->SetScaleMax({ 1.2f,1.2f,1.2f });
			newEmitter->SetRotateMin({ 0.0f,0.0f,0.0f });
			newEmitter->SetRotateMax({ 0.0f,0.0f,6.28f });
			newEmitter->SetVelocityMin(minVelocity);
			newEmitter->SetVelocityMax(maxVelocity);
			particleSystem_->AddParticleEmitter(newEmitter);
		}

		//移動処理
		player_->Move(velocity_);

		//敵の座標を取得
		Vector3 enemyPosition = GameObjectManager::GetInstance()->GetConstGameObject<Enemy>("")->GetHipWorldPosition();

		//プレイヤーの座標を取得
		Vector3 playerPosition = player_->GetHipWorldPosition();

		//敵と一定距離近づいたら速度を0にする
		if (Mathf::Length(enemyPosition - playerPosition) < player_->GetDashParameters().proximityDistance)
		{
			velocity_ = { 0.0f,0.0f,0.0f };
		}

		//エミッターを移動させる
		if (ParticleEmitter* emitter = particleSystem_->GetParticleEmitter("Dash"))
		{
			emitter->SetTranslate(player_->GetHipWorldPosition());
		}
	}
	else if (phaseName_ == "Recovery")
	{
		//フェーズが変わったとき
		if (prePhaseName_ != phaseName_)
		{
			//ダッシュのフラグを折る
			player_->SetActionFlag(Player::ActionFlag::kDashing, false);

			//プレイヤーを描画させる
			player_->SetIsVisible(true);

			//エミッターを削除
			if (ParticleEmitter* emitter = particleSystem_->GetParticleEmitter("Dash"))
			{
				emitter->SetIsDead(true);
			}

			//加速フィールドを削除
			if (AccelerationField* accelerationField = particleSystem_->GetAccelerationField("Dash"))
			{
				accelerationField->SetIsDead(true);
			}

			//アニメーションを変更
			player_->SetAnimationName("Armature.001|mixamo.com|Layer0.015");
			player_->SetIsAnimationStop(false);

			//RadialBlurを切る
			PostEffects::GetInstance()->GetRadialBlur()->SetIsEnable(false);
		}

		//アニメーションが終了していたら
		if (player_->GetIsAnimationEnd())
		{
			//アニメーション後の座標を代入
			Vector3 hipPosition = player_->GetHipWorldPosition();
			hipPosition.y = 0.0f;
			player_->SetPosition(hipPosition);

			//通常状態に戻す
			player_->ChangeState(new PlayerStateRoot());
		}
	}
}