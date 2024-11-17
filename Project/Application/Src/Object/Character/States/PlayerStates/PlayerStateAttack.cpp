#include "PlayerStateAttack.h"
#include "Application/Src/Object/Character/Player/Player.h"

int32_t PlayerStateAttack::comboIndex = 0;

void PlayerStateAttack::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーション名を設定
	animationName_ = character_->GetPosition().y > 0.0f ? SetAerialAnimationName() : SetGroundAnimationName();

	//アニメーションブレンドを無効にする
	character_->GetAnimator()->SetIsBlending(false);

	//攻撃アニメーションの再生とアニメーションコントローラーを取得
	SetAnimationControllerAndPlayAnimation(animationName_);
}

void PlayerStateAttack::Update()
{
	//アニメーションイベントを実行
	UpdateAnimationState();

	//コンボインデックスの更新
	UpdateComboIndex();

	//アニメーションが終了していた場合コンボインデックスをリセットして状態遷移する
	if (character_->GetAnimator()->GetIsAnimationFinished())
	{
		comboIndex = 0;
		HandleStateTransition();
	}
}

void PlayerStateAttack::OnCollision(GameObject* other)
{
	//衝突処理
	character_->ProcessCollisionImpact(other, true);
}

const std::string PlayerStateAttack::SetGroundAnimationName() const
{
	return "GroundAttack" + std::to_string(comboIndex + 1);
}

const std::string PlayerStateAttack::SetAerialAnimationName()const 
{
	return "AerialAttack" + std::to_string(comboIndex + 1);
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
	//それ以外の状態に遷移した場合はコンボをリセット
	else if(!comboNext_ && HasStateTransitioned())
	{
		comboIndex = 0;
	}
}