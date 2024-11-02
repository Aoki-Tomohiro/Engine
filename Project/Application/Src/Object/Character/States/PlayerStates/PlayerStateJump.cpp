#include "PlayerStateJump.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateRoot.h"

void PlayerStateJump::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//スティックの入力の強さを計算
	float inputLength = Mathf::Length({ input_->GetLeftStickX(), 0.0f, input_->GetLeftStickY() });

	//ジャンプのアニメーションとアニメーションイベントを設定
	ConfigureJumpAnimationAndEvents(inputLength);
}

void PlayerStateJump::Update()
{
	//重力加速度ベクトルの設定
	Vector3 accelerationVector = { 0.0f, character_->GetGravityAcceleration(), 0.0f };

	//速度に重力加速度を加算
	processedVelocityData_.velocity += accelerationVector * GameTimer::GetDeltaTime();

	//アニメーションイベントを実行
	UpdateAnimationState();

	//状態遷移
	HandleStateTransition();
}

void PlayerStateJump::HandleStateTransition()
{
	//プレイヤーを取得
	Player* player = GetPlayer();

	//座標を取得
	Vector3 playerPosition = character_->GetPosition();

	//プレイヤーが地面についた場合
	if (playerPosition.y <= 0.0f)
	{
		//プレイヤーが地面に埋まらないように座標を補正
		playerPosition.y = 0.0f;
		character_->SetPosition(playerPosition);

		//通常状態に遷移
		player->ChangeState(new PlayerStateRoot());
	}
}

void PlayerStateJump::OnCollision(GameObject* other)
{
	//衝突処理
	GetCharacter()->ProcessCollisionImpact(other, true);
}

void PlayerStateJump::ConfigureJumpAnimationAndEvents(const float inputLength)
{
	//プレイヤーを取得
	Player* player = GetPlayer();

	//スティック入力の状態に応じてアニメーションとイベントを選択
	std::string animationName = (inputLength > player->GetRootParameters().walkThreshold) ? "Jump2" : "Jump1";

	//アニメーションイベントを設定
	animationController_ = &character_->GetEditorManager()->GetCombatAnimationEditor()->GetAnimationController("Player", animationName);

	//アニメーションを再生
	character_->GetAnimator()->PlayAnimation(animationName, 1.0f, false);
}

void PlayerStateJump::InitializeVelocityMovement(const VelocityMovementEvent* velocityMovementEvent)
{
	//基底クラスの呼び出し
	IPlayerState::InitializeVelocityMovement(velocityMovementEvent);

	//ジャンプの初速度を設定
	processedVelocityData_.velocity.y = velocityMovementEvent->velocity.y;
}