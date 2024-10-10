#include "PlayerStateFalling.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateRoot.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateFallingAttack.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateChargeMagicAttack.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateDash.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateStun.h"

void PlayerStateFalling::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションの状態の取得
	animationState_ = player_->GetCombatAnimationEditor()->GetAnimationState("Falling");

	//アニメーションブレンドを有効化する
	player_->GetAnimator()->SetIsBlending(true);

	//落下アニメーションを再生
	player_->GetAnimator()->PlayAnimation("Falling", 1.0f, false);
}

void PlayerStateFalling::Update()
{
	//アニメーションによる座標のずれを補正
	player_->CorrectAnimationOffset();

	//プレイヤーが着地している場合の処理
	if (isLanding_)
	{
		HandleLanding();
		return;
	}
	//AボタンとXボタンを押したとき
	else if (input_->IsPressButton(XINPUT_GAMEPAD_A) && input_->IsPressButton(XINPUT_GAMEPAD_X))
	{
		//落下攻撃状態に遷移
		player_->ChangeState(new PlayerStateFallingAttack());
		return;
	}
	//Bボタンを押したとき
	else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
	{
		//ダッシュ状態に遷移
		player_->ChangeState(new PlayerStateDash());
		return;
	}
	//Yボタンを離した時
	else if (input_->IsPressButtonExit(XINPUT_GAMEPAD_Y) && player_->GetActionFlag(Player::ActionFlag::kChargeMagicAttackEnabled))
	{
		//溜め魔法攻撃状態に遷移
		player_->ChangeState(new PlayerStateChargeMagicAttack());
		return;
	}

	//アニメーションの更新
	CheckAndPauseAnimation();

	//プレイヤーの落下処理
	ApplyGravity();

	//プレイヤーが地面についたらを確認
	CheckLanding();
}

void PlayerStateFalling::OnCollision(GameObject* other)
{
	//衝突処理
	player_->ProcessCollisionImpact(other, true);
}

void PlayerStateFalling::ApplyGravity()
{
	//重力加速度ベクトルの設定
	Vector3 accelerationVector = { 0.0f, player_->GetGravityAcceleration(), 0.0f};

	//速度に重力加速度を加算
	velocity_ += accelerationVector * GameTimer::GetDeltaTime();

	//移動処理
	player_->Move(velocity_);
}

void PlayerStateFalling::CheckAndPauseAnimation()
{
	//アニメーターを取得
	AnimatorComponent* animator = player_->GetAnimator();

	//現在のアニメーション時間を取得
	float animationTime = animator->GetIsBlendingCompleted() ? animator->GetCurrentAnimationTime() : animator->GetNextAnimationTime();

	//アニメーション時間が指定したフェーズの終了時間を超えた場合
	if (animationTime > animationState_.phases[0].duration)
	{
		//落下アニメーションを一時停止
		animator->PauseAnimation();
	}
}

void PlayerStateFalling::CheckLanding()
{
	//プレイヤーの座標を取得
	Vector3 position = player_->GetPosition();

	//プレイヤーが地面についた場合
	if (position.y <= 0.0f)
	{
		//着地フラグを立てる
		isLanding_ = true;

		//プレイヤーが地面に埋まらないように座標を補正
		position.y = 0.0f;
		player_->SetPosition(position);

		//パーティクルを出す
		CreateLandingParticles();

		//落下アニメーションを一時停止を解除
		player_->GetAnimator()->ResumeAnimation();
	}
}

void PlayerStateFalling::HandleLanding()
{
	//アニメーションの再生が終了していたら通常状態に戻す
	if (Mathf::Length(GetInputValue()) > player_->GetRootParameters().walkThreshold || player_->GetAnimator()->GetIsAnimationFinished())
	{
		player_->ChangeState(new PlayerStateRoot());
	}
}

Vector3 PlayerStateFalling::GetInputValue()
{
	//スティックの入力を取得
	return { input_->GetLeftStickX(), 0.0f, input_->GetLeftStickY() };
}

void PlayerStateFalling::CreateLandingParticles()
{
	//着地パーティクルの生成
	ParticleEmitter* newEmitter = EmitterBuilder().SetColor({ 0.6f,0.6f,0.6f,1.0f }, { 0.6f,0.6f,0.6f,1.0f }).SetCount(100)
		.SetEmitterLifeTime(0.01f).SetEmitterName("Landing").SetFrequency(1.0f).SetLifeTime(0.3f, 0.5f).SetRadius(0.0f)
		.SetRotate({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }).SetScale({ 0.4f,0.4f,0.4f }, { 0.6f,0.6f,0.6f }).SetTranslation(player_->GetPosition())
		.SetVelocity({ -0.1f,0.02f,-0.1f }, { 0.1f,0.04f,0.1f }).Build();

	//パーティクルシステムにエミッターを追加
	player_->AddParticleEmitter("Smoke", newEmitter);
}