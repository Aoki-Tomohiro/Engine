#include "PlayerStateAttack.h"
#include "Application/Src/Object/Character/Player/Player.h"

int32_t PlayerStateAttack::comboIndex = 0;

void PlayerStateAttack::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//空中か地上かによってアニメーション名を決定
	animationName_ = DetermineAnimationName();

	//アニメーションブレンドを無効にする
	character_->GetAnimator()->SetIsBlending(false);

	//攻撃フラグを立てる
	GetPlayer()->SetActionFlag(Player::ActionFlag::kIsAttacking, true);

	//攻撃アニメーションの再生とアニメーションコントローラーを取得
	SetAnimationControllerAndPlayAnimation(animationName_);

	//ダッシュ攻撃の処理を実行
	HandleDashAttack();
}

void PlayerStateAttack::Update()
{
	//アニメーションイベントを実行
	UpdateAnimationState();

	//コンボインデックスの更新
	UpdateComboIndex();

	//アニメーションが終了していた場合
	if (character_->GetAnimator()->GetIsAnimationFinished())
	{
		//攻撃フラグをリセット
		GetPlayer()->SetActionFlag(Player::ActionFlag::kIsAttacking, false);
		//コンボをリセット
		comboIndex = 0;
		//デフォルトの状態に遷移
		HandleStateTransition();
	}
}

void PlayerStateAttack::OnCollision(GameObject* other)
{
	//衝突処理
	character_->ProcessCollisionImpact(other, true);
	//コンボをリセット
	comboIndex = 0;
}

std::string PlayerStateAttack::DetermineAnimationName() const
{
	//現在の高さを確認し、空中攻撃か地上攻撃かを決定
	return character_->GetPosition().y > 0.0f ? GetAerialAnimationName() : GetGroundAnimationName();
}

const std::string PlayerStateAttack::GetGroundAnimationName() const
{
	return "GroundAttack" + std::to_string(comboIndex + 1);
}

const std::string PlayerStateAttack::GetAerialAnimationName()const
{
	return "AerialAttack" + std::to_string(comboIndex + 1);
}

void PlayerStateAttack::HandleDashAttack()
{
	//ダッシュ攻撃が有効化されている場合
	if (GetPlayer()->GetActionFlag(Player::ActionFlag::kDashAttackEnabled))
	{
		//フラグを無効化
		GetPlayer()->SetActionFlag(Player::ActionFlag::kDashAttackEnabled, false);

		//アニメーション開始時間を設定
		float startTime = animationName_.find("GroundAttack") != std::string::npos ? groundAttackStartTime_ : aerialAttackStartTime_;
		character_->GetAnimator()->SetCurrentAnimationTime(startTime);
	}
}

void PlayerStateAttack::UpdateComboIndex()
{
	//攻撃状態に遷移されたかどうかを確認
	if (!comboNext_ && HasStateTransitioned("Attack"))
	{
		//次のコンボを有効にする
		comboNext_ = true;
		//コンボインデックスをインクリメント
		++comboIndex;
	}
	//攻撃以外の状態に遷移されていた場合
	else if(!comboNext_ && HasStateTransitioned())
	{
		//攻撃フラグをリセット
		GetPlayer()->SetActionFlag(Player::ActionFlag::kIsAttacking, false);
		//コンボをリセット
		comboIndex = 0;
	}
}