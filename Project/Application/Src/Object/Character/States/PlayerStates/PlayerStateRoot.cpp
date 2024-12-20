/**
 * @file PlayerStateRoot.cpp
 * @brief プレイヤーの通常状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#include "PlayerStateRoot.h"
#include "Application/Src/Object/Character/Player/Player.h"

void PlayerStateRoot::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションブレンドを有効にする
	character_->GetAnimator()->SetIsBlending(true);

	//アニメーションブレンドの時間を設定
	character_->GetAnimator()->SetBlendDuration(0.2f);

	//ロックオン中の場合は敵の方向に回転させる
	if (GetPlayer()->GetLockon()->ExistTarget())
	{
		GetPlayer()->LookAtEnemy();
	}
}

void PlayerStateRoot::Update()
{
	//ゲームが終了していた場合は処理を飛ばす
	if (character_->GetIsGameFinished())
	{
		SetIdleAnimationIfNotPlaying();
		return;
	}

	//アニメーションイベントを実行
	if (!currentAnimationName_.empty())
	{
		UpdateAnimationState();
	}

	//QTEが受付中の場合はスキップ
	for (const ProcessedQTEData& qteData : processedQTEDatas_)
	{
		if (qteData.isQTEActive) return;
	}

	//スティックの入力を取得
	Vector3 inputValue = { input_->GetLeftStickX(), 0.0f, input_->GetLeftStickY() };

	//入力ベクトルの長さを計算
	float inputLength = Mathf::Length(inputValue);

	//入力値が歩行の閾値を超えている場合
	if (inputLength > GetPlayer()->GetRootParameters().walkThreshold)
	{
		//移動処理
		Move(inputValue, inputLength);
	}
	else
	{
		//現在のアニメーションがIdleではない場合再生する
		SetIdleAnimationIfNotPlaying();
	}

	//状態遷移処理
	HandleStateTransition();
}

void PlayerStateRoot::OnCollision(GameObject* other)
{
	//衝突処理
	GetCharacter()->ProcessCollisionImpact(other, true);
}

void PlayerStateRoot::HandleStateTransitionInternal()
{
	//遷移可能なアクション一覧
	const std::vector<std::string> actions = { "Jump", "Dodge", "Dash", "Attack", "Magic", "ChargeMagic", "Ability1", "Ability2" };

	//アクションリストをループし、ボタン入力をチェック
	for (const auto& action : actions)
	{
		//対応するボタンが押されていた場合
		if (GetPlayer()->IsActionExecutable(action))
		{
			//対応する状態に遷移
			character_->ChangeState(action);
			return;
		}
	}
}

void PlayerStateRoot::SetIdleAnimationIfNotPlaying()
{
	//現在のアニメーションが通常状態ではない場合
	if (currentAnimationName_ != "Idle")
	{
		//現在のアニメーションを通常状態に変更
		currentAnimationName_ = "Idle";
		//歩きの閾値を超えていない場合は待機アニメーションを設定
		SetAnimationControllerAndPlayAnimation(currentAnimationName_, true);
	}
}

void PlayerStateRoot::Move(const Vector3& inputValue, const float inputLength)
{
	//プレイヤーを取得
	Player* player = GetPlayer();

	//入力値が走りの閾値を超えているかチェック
	bool isRunning = inputLength > player->GetRootParameters().runThreshold;

	//アニメーションの更新
	UpdateAnimation(inputValue, isRunning);

	//移動速度を設定
	float moveSpeed = isRunning ? player->GetRootParameters().runSpeed : player->GetRootParameters().walkSpeed;
	//入力ベクトルを正規化し速度を掛ける
	Vector3 velocity = Mathf::Normalize(inputValue) * moveSpeed;

	//移動ベクトルにカメラの回転を適用
	velocity = Mathf::RotateVector(velocity, player->GetCameraController()->GetCamera().quaternion_);
	//水平方向に移動させるのでY成分を0にする
	velocity.y = 0.0f;

	//プレイヤーを移動させる
	player->Move(velocity);

	//プレイヤーの回転を更新
	UpdateRotation(velocity);
}

void PlayerStateRoot::UpdateAnimation(const Vector3& inputValue, bool isRunning)
{
	//アニメーション名を格納するための変数
	std::string animationName;

	//ロックオン中のアニメーション決定
	if (GetPlayer()->GetLockon()->ExistTarget())
	{
		//ロックオン中で、プレイヤーが走っているかどうかでアニメーションを決定
		if (isRunning)
		{
			//走行中の場合、入力ベクトルに基づいた走りアニメーションを選択
			animationName = DetermineRunningAnimation(inputValue);
		}
		else
		{
			//歩行中の場合、入力ベクトルに基づいた歩きアニメーションを選択
			animationName = DetermineWalkingAnimation(inputValue);
		}
	}
	else
	{
		//ロックオンしていない場合、走行または歩行に応じたデフォルトアニメーションを選択
		animationName = isRunning ? "Run1" : "Walk1";
	}

	//現在のアニメーションが変わっていた場合
	if (currentAnimationName_ != animationName)
	{
		//現在のアニメーションを更新
		currentAnimationName_ = animationName;
		//歩きの閾値を超えていない場合は待機アニメーションを設定
		SetAnimationControllerAndPlayAnimation(currentAnimationName_, true);
	}
}

const std::string PlayerStateRoot::DetermineWalkingAnimation(const Vector3& inputValue) const
{
	//入力ベクトルの方向に応じて歩行アニメーションを決定
	if (std::abs(inputValue.z) > std::abs(inputValue.x))
	{
		//前進または後退のアニメーション
		return (inputValue.z > 0.0f) ? "Walk1" : "Walk2";
	}
	else
	{
		//左右移動のアニメーション
		return (inputValue.x < 0.0f) ? "Walk3" : "Walk4";
	}
}

const std::string PlayerStateRoot::DetermineRunningAnimation(const Vector3& inputValue) const
{
	//入力ベクトルの方向に応じて走行アニメーションを決定
	if (std::abs(inputValue.z) > std::abs(inputValue.x))
	{
		//前進または後退のアニメーション
		return (inputValue.z > 0.0f) ? "Run1" : "Run2";
	}
	else
	{
		//左右移動のアニメーション
		return (inputValue.x < 0.0f) ? "Run3" : "Run4";
	}
}

void PlayerStateRoot::UpdateRotation(const Vector3& velocity)
{
	//プレイヤーを取得
	Player* player = GetPlayer();

	//ロックオン中の場合は敵の方向に向かせる
	if (player->GetLockon()->ExistTarget())
	{
		player->LookAtEnemy();
	}
	//ロックオンしていない場合は速度ベクトルの方向に回転
	else
	{
		player->Rotate(Mathf::Normalize(velocity));
	}
}