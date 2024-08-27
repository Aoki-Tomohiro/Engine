#include "PlayerStateDash.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Enemy/Enemy.h"

void PlayerStateDash::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//ダッシュのフラグを設定
	player_->SetActionFlag(Player::ActionFlag::kDashing, true);

	//アニメーションの状態の取得
	animationState_ = player_->GetAnimationStateManager()->GetAnimationState("Dash");

	//ダッシュ開始時のアニメーションを再生
	player_->PlayAnimation("DashStart", 1.0f, false);

	//ロックオン中の場合敵の方向に向かってダッシュ
	if (player_->GetLockon()->ExistTarget())
	{
		HandleLockon();
	}
	//ロックオンしていない場合前方に向かってダッシュ
	else
	{
		HandleNonLockon();
	}

	//パーティクルシステムの初期化
	InitializeParticleSystem();
}

void PlayerStateDash::Update()
{
	//前のアニメーションのフェーズを更新
	prePhaseIndex_ = currentPhaseIndex_;

	//アニメーションの時間を進める
	animationTime_ += GameTimer::GetDeltaTime();

	//アニメーションのフェーズの更新
	UpdateAnimationPhase();

	//現在のフェーズに応じた処理
	switch (currentPhaseIndex_)
	{
	case kCharge:
		break;
	case kDash:
		//ダッシュ状態の更新
		DashUpdate();
		break;
	case kRecovery:
		//硬直状態の更新
		RecoveryUpdate();
		break;
	}
}

void PlayerStateDash::HandleLockon()
{
	//敵の座標を取得
	Vector3 enemyPosition = GameObjectManager::GetInstance()->GetMutableGameObject<Enemy>("")->GetHipWorldPosition();
	//プレイヤーの座標を取得
	Vector3 playerPosition = player_->GetHipWorldPosition();

	//敵とプレイヤーの方向を計算
	Vector3 sub = Mathf::Normalize(enemyPosition - playerPosition);
	//Y軸の影響を排除して水平移動に限定
	sub.y = 0.0f;

	//計算した方向と速度でダッシュベクトルを設定
	velocity_ = sub * player_->GetDashParameters().dashSpeed;
	//プレイヤーを敵の方向に向ける
	player_->Rotate(sub);
}

void PlayerStateDash::HandleNonLockon()
{
	//プレイヤーの前方にダッシュ
	velocity_ = Mathf::RotateVector({ 0.0f, 0.0f, -1.0f }, player_->GetDestinationQuaternion());
	velocity_ = Mathf::Normalize(velocity_) * player_->GetDashParameters().dashSpeed;
}

void PlayerStateDash::InitializeParticleSystem()
{
	//テクスチャの読み込み
	TextureManager::Load("smoke_01.png");

	//パーティクルの初期化
	particleSystem_ = ParticleManager::Create("Dash");
	particleSystem_->SetTexture("smoke_01.png");
}

void PlayerStateDash::UpdateAnimationPhase()
{
	//アニメーションのフェーズの更新
	if (currentPhaseIndex_ < animationState_.phases.size() - 1 && animationTime_ > animationState_.phases[currentPhaseIndex_].animationTime)
	{
		currentPhaseIndex_++;
	}
}

void PlayerStateDash::DashUpdate()
{
	//フェーズが切り替わった場合
	if (prePhaseIndex_ != currentPhaseIndex_)
	{
		//プレイヤーを一時的に非表示にする
		player_->SetIsVisible(false);

		//ダッシュのパーティクルを生成
		SpawnDashParticles();
	}

	//移動処理
	player_->Move(velocity_);

	//エミッターの座標を更新
	UpdateEmitterPosition();

	////敵の座標を取得
	//Vector3 enemyPosition = GameObjectManager::GetInstance()->GetMutableGameObject<Enemy>("")->GetHipWorldPosition();

	////プレイヤーの座標を取得
	//Vector3 playerPosition = player_->GetHipWorldPosition();

	////敵と一定距離近づいたら速度を0にする
	//if (Mathf::Length(enemyPosition - playerPosition) < player_->GetDashParameters().proximityDistance)
	//{
	//	velocity_ = { 0.0f,0.0f,0.0f };
	//}
}

void PlayerStateDash::RecoveryUpdate()
{
	//フェーズが変わったとき
	if (prePhaseIndex_ != currentPhaseIndex_)
	{
		//ダッシュの状態をリセット
		ResetDashFlags();

		//ダッシュ終了時のアニメーションを再生
		player_->StopAnimation();
		player_->PlayAnimation("DashEnd", 1.0f, false);

		//ラジアルブラーを切る
		PostEffects::GetInstance()->GetRadialBlur()->SetIsEnable(false);
	}
	else if (player_->GetIsAnimationFinished())
	{
		//アニメーションが終了したら通常状態に戻す
		player_->ChangeState(new PlayerStateRoot());
	}
}

void PlayerStateDash::SpawnDashParticles()
{
	//ダッシュパーティクルの生成
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

void PlayerStateDash::UpdateEmitterPosition()
{
	//エミッターの位置をプレイヤーの位置に更新
	if (ParticleEmitter* emitter = particleSystem_->GetParticleEmitter("Dash"))
	{
	    emitter->SetTranslate(player_->GetHipWorldPosition());
	}
}

void PlayerStateDash::ResetDashFlags()
{
	//プレイヤーを再表示
	player_->SetIsVisible(true);
	//ダッシュのフラグを解除
	player_->SetActionFlag(Player::ActionFlag::kDashing, false);

	// パーティクルエミッターと加速フィールドを削除
	if (ParticleEmitter* emitter = particleSystem_->GetParticleEmitter("Dash"))
	{
	    emitter->SetIsDead(true);
	}
	if (AccelerationField* accelerationField = particleSystem_->GetAccelerationField("Dash"))
	{
	    accelerationField->SetIsDead(true);
	}
}