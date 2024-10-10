#include "PlayerStateSpinAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateFalling.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateAttack.h"
#include "Application/Src/Object/Weapon/Weapon.h"

void PlayerStateSpinAttack::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//アニメーションの状態を取得して設定
	animationState_ = player_->GetCombatAnimationEditor()->GetAnimationState("SpinAttack");

	//プレイヤーのアニメーションを再生
	player_->GetAnimator()->PlayAnimation("SpinAttack", 1.0f, false);

	//攻撃パラメーターを取得
	const auto& attackParameters = player_->GetSpinAttackParameters();

	//回転開始時のプレイヤーの位置を設定
	startPosition_ = player_->GetPosition();

	//目標とする座標を設定
	targetPosition_ = player_->GetPosition() + Vector3{ 0.0f,attackParameters.riseHeight,0.0f };

	//軌跡エフェクトを有効にする
	Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>("PlayerWeapon");
	weapon->SetIsTrailActive(true);

	//武器にパラメーターを設定
	player_->ApplyParametersToWeapon("PlayerWeapon", animationState_.phases[0]);
}

void PlayerStateSpinAttack::Update()
{
	//攻撃状態への遷移要求をチェック
	CheckForAttackStateTransition();

	//現在の回転時間を進める
	currentRotateTime_ += GameTimer::GetDeltaTime();

	//アニメーションの状態の第一フェーズの継続時間以内かどうかをチェック
	if (currentRotateTime_ < animationState_.phases[0].duration)
	{
		//回転処理
		UpdateRotation();

		//空中での位置更新処理
		UpdatePosition();

		//武器のヒット判定処理
		HandleWeaponHit();
	}
	else
	{
		//回転攻撃終了後の処理
		FinalizeSpinAttack();
	}
}

void PlayerStateSpinAttack::OnCollision(GameObject* other)
{
	//衝突処理
	player_->ProcessCollisionImpact(other, false);
}

void PlayerStateSpinAttack::CheckForAttackStateTransition()
{
	//Xボタンが押されている場合攻撃状態への遷移要求を設定
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
	{
		isAttackStateTransitionRequested_ = true;
	}
}

void PlayerStateSpinAttack::UpdateRotation()
{
	//攻撃パラメーターを取得
	const auto& attackParameters = player_->GetSpinAttackParameters();

	//回転時間を正規化して0から1の範囲に収める
	float normalizeTime = std::min<float>(currentRotateTime_ / attackParameters.totalDuration, 1.0f);

	//イージング関数を使用して回転のイージング値を計算
	float easingValue = Mathf::EaseInOutSine(normalizeTime);

	//累積のイージングされた回転量を計算
	float totalEasedRotation = attackParameters.totalRotation * easingValue;

	//フレームごとの回転量を計算
	float frameRotation = totalEasedRotation - preEasedRotation;

	//累積回転量を保存
	preEasedRotation = totalEasedRotation;

	//現在のクォータニオンに新しい回転を適用
	Quaternion newRotation = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f, 1.0f, 0.0f }, frameRotation);
	player_->SetDestinationQuaternion(Mathf::Normalize(player_->GetDestinationQuaternion() * newRotation));

	//アニメーション速度を設定
	player_->GetAnimator()->SetCurrentAnimationSpeed(animationState_.phases[0].duration / attackParameters.totalDuration);
}

void PlayerStateSpinAttack::UpdatePosition()
{
	//攻撃パラメーターを取得
	const auto& attackParameters = player_->GetSpinAttackParameters();

	//現在の回転時間を正規化して0から1の範囲に収める
	float normalizeTime = std::min<float>(currentRotateTime_ / attackParameters.totalDuration, 1.0f);

	//イージング関数を使用して移動位置を計算
	Vector3 movedPosition = startPosition_ + (targetPosition_ - startPosition_) * Mathf::EaseInOutSine(normalizeTime);

	//プレイヤーの位置を設定
	player_->SetPosition(movedPosition);
}

void PlayerStateSpinAttack::HandleWeaponHit()
{
	//プレイヤーの武器を取得
	Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>("PlayerWeapon");

	//現在のフェーズでのヒット数が上限に達していない場合
	if (hitCount_ < animationState_.phases[0].attackSettings.hitCount)
	{
		//ヒットタイマーを進める
		hitTimer_ += GameTimer::GetDeltaTime();

		//ヒットタイマーがヒット間隔を超えた場合、攻撃判定を付ける
		if (hitTimer_ > animationState_.phases[0].attackSettings.hitInterval)
		{
			hitTimer_ = 0.0f;
			weapon->SetIsAttack(true);
		}
		else
		{
			weapon->SetIsAttack(false);
		}

		//武器が敵にヒットしたかをチェック
		if (weapon->GetIsHit())
		{
			hitCount_++; //ヒットカウントを増加
		}
	}
	else
	{
		//ヒット数が上限に達した場合攻撃状態を無効にする
		weapon->SetIsAttack(false);
	}
}

void PlayerStateSpinAttack::FinalizeSpinAttack()
{
	//スピンアタック終了後のアニメーション速度を設定
	player_->GetAnimator()->SetCurrentAnimationSpeed(2.0f);

	//武器の攻撃判定を無効にする
	Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>("PlayerWeapon");
	weapon->SetIsTrailActive(false);
	weapon->SetIsAttack(false);

	//攻撃状態への遷移フラグが立っている場合
	if (isAttackStateTransitionRequested_)
	{
		//スキルのクールダウンをリセット
		player_->ResetCooldown(Skill::kSpinAttack);
		//攻撃状態に遷移
		player_->ChangeState(new PlayerStateAttack());
	}
	else
	{
		//スキルのクールダウンをリセット
		player_->ResetCooldown(Skill::kSpinAttack);
		//プレイヤーの状態を通常の状態に戻す
		player_->ChangeState(new PlayerStateFalling());
	}
}