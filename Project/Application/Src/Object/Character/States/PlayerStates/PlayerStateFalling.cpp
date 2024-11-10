#include "PlayerStateFalling.h"
#include "Application/Src/Object/Character/Player/Player.h"

void PlayerStateFalling::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションブレンドを有効にする
	character_->GetAnimator()->SetIsBlending(true);

	//落下アニメーションの再生とアニメーションコントローラーを取得
	SetAnimationControllerAndPlayAnimation("Falling");
}

void PlayerStateFalling::Update()
{
	//アニメーションイベントを実行
	UpdateAnimationState();

	//重力を適用
	ApplyGravity();

	//空中または地面にいる場合の処理
	CheckAndHandleLanding();

	//デフォルトの状態に遷移
	if (character_->GetAnimator()->GetIsAnimationFinished())
	{
		HandleStateTransition();
	}
}

void PlayerStateFalling::OnCollision(GameObject* other)
{
	//衝突処理
	character_->ProcessCollisionImpact(other, true);
}

void PlayerStateFalling::ApplyGravity()
{
	//着地している場合は処理を飛ばす
	if (isLanding_) { return; };

	//重力加速度ベクトルの設定
	Vector3 accelerationVector = { 0.0f, character_->GetGravityAcceleration(), 0.0f };

	//速度に重力加速度を加算
	processedVelocityDatas_[0].velocity += accelerationVector * GameTimer::GetDeltaTime();
}

void PlayerStateFalling::CheckAndHandleLanding()
{
	//現在のキャラクターの位置を取得
	Vector3 currentPosition = character_->GetPosition();

	//もしキャラクターが地面より下にいる場合
	if (currentPosition.y < 0.0f && !isLanding_)
	{
		ProcessLanding(currentPosition);
	}
	//プレイヤーが空中にいる場合
	else if (!processedVelocityDatas_[0].isActive)
	{
		HandleAirborneAnimation();
	}
}

void PlayerStateFalling::ProcessLanding(const Vector3& landingPosition)
{
	//着地フラグの設定
	isLanding_ = true;

	//速度を0にリセット
	processedVelocityDatas_[0].velocity = { 0.0f, 0.0f, 0.0f };

	//キャラクターの位置を地面に揃える
	character_->SetPosition({ landingPosition.x, 0.0f, landingPosition.z });

	//着地パーティクルを生成
	character_->GetEditorManager()->GetParticleEffectEditor()->CreateParticles("Landing", landingPosition, character_->GetQuaternion());
}

void PlayerStateFalling::HandleAirborneAnimation()
{
	//着地フラグの状態によってアニメーションを管理
	if (!isLanding_)
	{
		//空中でアニメーションを一時停止
		character_->GetAnimator()->PauseAnimation();
	}
	else
	{
		//着地後アニメーションを再開
		character_->GetAnimator()->ResumeAnimation();
	}
}