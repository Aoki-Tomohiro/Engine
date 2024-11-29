#include "PlayerStateDodge.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"

void PlayerStateDodge::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションブレンドを有効にする
	character_->GetAnimator()->SetIsBlending(true);

	//アニメーションブレンドの時間を設定
	character_->GetAnimator()->SetBlendDuration(0.2f);

	//コライダーを無効化する
	character_->GetCollider()->SetCollisionEnabled(false);

	//スティックの入力の強さを計算
	float inputLength = Mathf::Length({ input_->GetLeftStickX(), 0.0f, input_->GetLeftStickY() });

	//回避のアニメーションとアニメーションイベントを設定
	ConfigureDodgeAnimationAndEvents(inputLength);
}

void PlayerStateDodge::Update()
{
	//アニメーションイベントを実行
	UpdateAnimationState();

	//状態遷移されていた場合はコライダーを有効化する
	if (HasStateTransitioned())
	{
		//コライダーを有効化する
		character_->GetCollider()->SetCollisionEnabled(true);
	}

	//アニメーションが終了していた場合
	if (character_->GetAnimator()->GetIsAnimationFinished())
	{
		//コライダーを有効化する
		character_->GetCollider()->SetCollisionEnabled(true);
		//デフォルトの状態に遷移
		HandleStateTransition();
	}
}

void PlayerStateDodge::ConfigureDodgeAnimationAndEvents(const float inputLength)
{
	//敵がジャスト回避受け付け中の場合
	if (GameObjectManager::GetInstance()->GetGameObject<Enemy>("Enemy")->GetActionFlag(Enemy::ActionFlag::kIsAttacking))
	{

	}

	//プレイヤーを取得
	Player* player = GetPlayer();

	//スティック入力の状態に応じてアニメーションとイベントを選択
	animationName_ = (inputLength > player->GetRootParameters().walkThreshold) ? "DodgeForward" : "DodgeBackward";

	//回避アニメーションの再生とアニメーションコントローラーを取得
	SetAnimationControllerAndPlayAnimation(animationName_);
}