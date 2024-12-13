/**
 * @file PlayerStateJump.cpp
 * @brief プレイヤーのジャンプ状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#include "PlayerStateJump.h"
#include "Application/Src/Object/Character/Player/Player.h"

void PlayerStateJump::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//スティックの入力の強さを計算
	float inputLength = Mathf::Length({ input_->GetLeftStickX(), 0.0f, input_->GetLeftStickY() });

	//アニメーションブレンドを無効にする
	character_->GetAnimator()->SetIsBlending(false);

	//ジャンプのアニメーションとアニメーションイベントを設定
	ConfigureJumpAnimationAndEvents(inputLength);
}

void PlayerStateJump::Update()
{
	//アニメーションイベントを実行
	UpdateAnimationState();

	//重力を適用
	ApplyGravity();

	//ロックオン中は敵の方向に向ける
	LookAtLockonTarget();

	//キャラクターが着地しているかを確認
	CheckLandingAndTransitionState();
}

void PlayerStateJump::OnCollision(GameObject* other)
{
	//衝突処理
	character_->ProcessCollisionImpact(other, true);
}

void PlayerStateJump::ConfigureJumpAnimationAndEvents(const float inputLength)
{
	//プレイヤーを取得
	Player* player = GetPlayer();

	//スティック入力の状態に応じてアニメーションとイベントを選択
	std::string animationName = (inputLength > player->GetRootParameters().walkThreshold) ? "Jump2" : "Jump1";

	//ジャンプアニメーションの再生とアニメーションコントローラーを取得
	SetAnimationControllerAndPlayAnimation(animationName);
}

void PlayerStateJump::ApplyGravity()
{
	//重力加速度ベクトルの設定
	Vector3 accelerationVector = { 0.0f, character_->GetGravityAcceleration(), 0.0f };

	//速度に重力加速度を加算
	if (!processedVelocityDatas_.empty())
	{
		processedVelocityDatas_[0].velocity += accelerationVector * GameTimer::GetDeltaTime() * character_->GetTimeScale();
	}
}

void PlayerStateJump::LookAtLockonTarget()
{
	//ロックオン中敵の方向を向ける
	if (GetPlayer()->GetLockon()->ExistTarget())
	{
		GetPlayer()->LookAtEnemy();
	}
}

void PlayerStateJump::CheckLandingAndTransitionState()
{
	//座標を取得
	Vector3 position = character_->GetPosition();

	//地面に着地していたら
	if (position.y < 0.0f)
	{
		//パーティクルを出す
		character_->GetEditorManager()->GetParticleEffectEditor()->CreateParticles("Landing", position, character_->GetQuaternion());

		//デフォルトの状態に戻す
		HandleStateTransition();
	}
}