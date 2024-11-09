#include "PlayerStateAttack.h"
#include "Application/Src/Object/Character/Player/Player.h"

int32_t PlayerStateAttack::comboIndex = 0;

void PlayerStateAttack::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーション名を設定
	animationName_ = character_->GetPosition().y > 0.0f ? SetAerialAnimationName() : SetGroundAnimationName();

	//ダッシュ開始時のアニメーションの再生とアニメーションコントローラーを取得
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
	GetPlayer()->SetActionFlag(Player::ActionFlag::kAerialAttack, true);
	return "AerialAttack" + std::to_string(comboIndex + 1);
}

void PlayerStateAttack::UpdateComboIndex()
{
	//全てのアニメーションキャンセルを確認
	for (const ProcessedCancelData& cancelData : processedCancelDatas_)
	{
		//アニメーションキャンセルが行われていた場合コンボインデックスを初期化
		if (cancelData.isCanceled)
		{
			if (cancelData.cancelActionName != "Attack")
			{
				comboIndex = 0;
			}
			else
			{
				++comboIndex;
			}
		}
	}

	//全ての先行入力を確認
	for (const ProcessedBufferedActionData& bufferedActionData : processedBufferedActionDatas_)
	{
		//先行入力されたアクションがAttackである場合
		if (!comboNext_ && bufferedActionData.isBufferedInputActive && bufferedActionData.bufferedActionName == "Attack")
		{
			//次のコンボを有効にする
			comboNext_ = true;
			//コンボインデックスをインクリメント
			++comboIndex;
		}
	}
}