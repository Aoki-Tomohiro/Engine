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
	//アニメーションブレンドの基本の値
	static const float kDefaultBlendDuration = 0.3f;

	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションブレンドを有効にする
	character_->GetAnimator()->SetIsBlending(true);

	//アニメーションブレンドの時間を設定
	character_->GetAnimator()->SetBlendDuration(kDefaultBlendDuration);

	//ロックオン中の場合は敵の方向に回転させる
	if (GetPlayer()->GetLockon()->ExistTarget())
	{
		GetPlayer()->LookAtEnemy();
	}
}

void PlayerStateRoot::Update()
{
	//反転入力を許容する閾値
	static const float kDotProductThreshold = 0.5f;  

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

	//スティックの入力を取得
	Vector3 inputValue = { input_->GetLeftStickX(), 0.0f, input_->GetLeftStickY() };
	float inputLength = Mathf::Length(inputValue); //入力ベクトルの長さを計算

	//歩行閾値を超えた場合は移動処理
	if (inputLength > GetPlayer()->GetRootParameters().walkThreshold)
	{
		Move(inputValue, inputLength);
	}
	//逆入力が検出されていない場合はIdleアニメーションを再生
	else if (inputLength <= GetPlayer()->GetRootParameters().walkThreshold || Mathf::Dot(previousInputValue_, inputValue) > kDotProductThreshold)
	{
		SetIdleAnimationIfNotPlaying();
	}

	//前回の入力を保存して次回の更新で比較
	previousInputValue_ = inputValue;

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
	const std::vector<std::string> actions = { "Jump", "Dodge", "Dash", "Attack", "Magic", "ChargeMagic", "LaunchAttack", "SpinAttack" };

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
		//移動速度をリセット
		currentMoveSpeed_ = 0.0f;
		//アニメーションを再生
		SetAnimationControllerAndPlayAnimation(currentAnimationName_, true); 
	}
}

void PlayerStateRoot::Move(const Vector3& inputValue, const float inputLength)
{
	//補間速度
	static const float kInterpolationSpeed = 0.1f;

	//プレイヤーを取得
	Player* player = GetPlayer();

	//走っているか歩いているかの判定
	bool isRunning = inputLength > player->GetRootParameters().runThreshold;
	//アニメーションの更新
	UpdateAnimation(inputValue, isRunning);

	//目標速度の設定
	float targetMoveSpeed = isRunning ? player->GetRootParameters().runSpeed : player->GetRootParameters().walkSpeed;
	//移動速度を補間
	currentMoveSpeed_ = Mathf::Lerp(currentMoveSpeed_, targetMoveSpeed, kInterpolationSpeed);  

	//入力ベクトルを正規化し速度を掛ける
	Vector3 velocity = Mathf::Normalize(inputValue) * currentMoveSpeed_;

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
	//アニメーション名を決定
	std::string animationName = GetAnimationNameBasedOnLockon(inputValue, isRunning);

	//現在のアニメーションが変わっていた場合
	if (currentAnimationName_ != animationName)
	{
		//対となるアニメーションに変更する場合は現在の移動速度をリセット
		if (!currentAnimationName_.empty() && IsOppositeAnimation(currentAnimationName_, animationName))
		{
			currentMoveSpeed_ = 0.0f;
		}
		//現在のアニメーションを更新
		currentAnimationName_ = animationName; 
		//新しいアニメーションを再生
		SetAnimationControllerAndPlayAnimation(currentAnimationName_, true); 
	}
}

std::string PlayerStateRoot::GetAnimationNameBasedOnLockon(const Vector3& inputValue, bool isRunning) const
{
	//ロックオン中かどうかでアニメーションを選択
	if (GetPlayer()->GetLockon()->ExistTarget())
	{
		return isRunning ? DetermineRunningAnimation(inputValue) : DetermineWalkingAnimation(inputValue);
	}
	//ロックオンしていない場合は通常の走行/歩行アニメーション
	return isRunning ? "Run1" : "Walk1";
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

bool PlayerStateRoot::IsOppositeAnimation(const std::string& currentAnimationName, const std::string& animationName)
{
	//待機アニメーションが設定されている場合は必ず逆方向として扱う
	if (currentAnimationName == "Idle" || animationName == "Idle") return true;

	//アニメーション名の末尾の数字を比較して逆方向かどうかを判定
	int currentNum = std::stoi(currentAnimationName.substr(currentAnimationName.size() - 1));
	int newNum = std::stoi(animationName.substr(animationName.size() - 1));

	//1と2、3と4が逆方向のアニメーションとして認識
	return (currentNum == 1 && newNum == 2) || (currentNum == 2 && newNum == 1) || (currentNum == 3 && newNum == 4) || (currentNum == 4 && newNum == 3);
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