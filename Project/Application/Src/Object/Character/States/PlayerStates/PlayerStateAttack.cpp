#include "PlayerStateAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateRoot.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateFalling.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateDash.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateLaunchAttack.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateSpinAttack.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateFallingAttack.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateChargeMagicAttack.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateStun.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Laser/Laser.h"
#include "Application/Src/Object/Pillar/Pillar.h"

void PlayerStateAttack::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//空中攻撃のフラグの設定
	player_->SetActionFlag(Player::ActionFlag::kAerialAttack, player_->GetPosition().y > 0.0f);

	//攻撃時のアニメーション状態を読み込む
	LoadAnimationStates(player_->GetActionFlag(Player::ActionFlag::kAerialAttack));

	//プレイヤーの攻撃タイプに応じて初期アニメーションを再生
	PlayAttackAnimation(player_->GetActionFlag(Player::ActionFlag::kAerialAttack));

	//ダッシュ攻撃のフラグが立っている場合はすぐに攻撃できるようにする
	InitializeDashAttack();

	//速度を計算
	CalculateVelocity();
}

void PlayerStateAttack::Update()
{
	//コンボの進行状況をチェック
	EvaluateComboProgress();

	//アニメーションの更新とフェーズの処理
	UpdateAnimationAndHandlePhase();

	//プレイヤーを敵に向けて回転
	RotatePlayerTowardsEnemy();

	//コンボ状態の更新
	UpdateComboState();
}

void PlayerStateAttack::OnCollision(GameObject* other)
{
	//アニメーションを停止
	player_->GetAnimator()->StopAnimation();

	//プレイヤーの位置をアニメーション後の位置に補正
	CorrectPlayerPosition();

	//衝突処理
	player_->ProcessCollisionImpact(other, true);
}

void PlayerStateAttack::PlayAttackAnimation(const bool isAerial)
{
	//アニメーションの名前を設定
	std::string animationName = (isAerial ? "AerialAttack" : "GroundAttack") + std::to_string(workAttack_.comboIndex + 1);
	//アニメーションの速度を設定
	float animationSpeed = isAerial ? 3.0f : 2.4f;
	//アニメーションを再生
	player_->GetAnimator()->PlayAnimation(animationName, animationSpeed, false);
}

void PlayerStateAttack::LoadAnimationStates(const bool isAerial)
{
	//攻撃のパラメーターを取得
	const auto& attackParameters = isAerial ? player_->GetAerialAttackParameters() : player_->GetGroundAttackParameters();

	//コンボの各段階に対応するアニメーション状態を取得して保存
	for (int32_t i = 0; i < attackParameters.comboNum; ++i)
	{
		std::string stateName = (isAerial ? "AerialAttack" : "GroundAttack") + std::to_string(i + 1);
		animationStates_.push_back(player_->GetCombatAnimationEditor()->GetAnimationState(stateName));
	}
}

void PlayerStateAttack::InitializeDashAttack()
{
	//ダッシュ攻撃のフラグが立っている場合はすぐに攻撃できるようにする
	if (player_->GetActionFlag(Player::ActionFlag::kDashAttackEnabled))
	{
		player_->SetActionFlag(Player::ActionFlag::kDashAttackEnabled, false);
		player_->GetAnimator()->SetNextAnimationTime(animationStates_[0].phases[0].duration);
	}
}

void PlayerStateAttack::CalculateVelocity()
{
	//移動速度を取得
	float moveSpeed = animationStates_[workAttack_.comboIndex].phases[workAttack_.inComboPhase].attackSettings.moveSpeed;
	//速度を設定
	velocity_ = Mathf::RotateVector({ 0.0f,0.0f,moveSpeed }, player_->GetDestinationQuaternion());
}

void PlayerStateAttack::EvaluateComboProgress()
{
	//攻撃パラメーターを取得
	const auto& attackParameters = player_->GetActionFlag(Player::ActionFlag::kAerialAttack) ? player_->GetAerialAttackParameters() : player_->GetGroundAttackParameters();

	//コンボ上限に達していないとき
	if (workAttack_.comboIndex < attackParameters.comboNum - 1)
	{
		//RTの入力の閾値
		const float kRightTriggerThreshold = 0.7f;

		//LTを押している場合
		if (input_->GetRightTriggerValue() > kRightTriggerThreshold)
		{
			//XボタンかYボタンが押されているかをチェック
			UpdateComboStateBasedOnButtonPress();
		}
		else
		{
			//Aボタンを押していない場合
			if (!input_->IsPressButton(XINPUT_GAMEPAD_A))
			{
				//攻撃ボタンをトリガーしたら
				if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
				{
					EnableCombo(kNormalCombo);
				}
				//ダッシュボタンをトリガーしたら
				else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
				{
					EnableCombo(kDash);
				}
				//Yボタンを離した時
				else if (input_->IsPressButtonExit(XINPUT_GAMEPAD_Y) && player_->GetActionFlag(Player::ActionFlag::kChargeMagicAttackEnabled))
				{
					EnableCombo(kChargeMagic);
				}
			}
			//Aボタンを押している場合
			else
			{
				//Xボタンも同時に押されていたら落下攻撃
				if (input_->IsPressButton(XINPUT_GAMEPAD_X) && player_->GetPosition().y != 0.0f)
				{
					EnableCombo(kFallingAttack);
				}
			}
		}
	}
}

void PlayerStateAttack::UpdateComboStateWithoutTrigger()
{
	//通常コンボ
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
	{
		EnableCombo(kNormalCombo);
	}
	//ダッシュ
	else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
	{
		EnableCombo(kDash);
	}
}

void PlayerStateAttack::UpdateComboStateBasedOnButtonPress()
{
	if (!input_->IsPressButton(XINPUT_GAMEPAD_A))
	{
		//打ち上げ攻撃
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) && player_->GetIsCooldownComplete(Skill::kLaunchAttack) && player_->GetPosition().y == 0.0f)
		{
			EnableCombo(kLaunchAttack);
		}
		//回転攻撃
		else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y) && player_->GetIsCooldownComplete(Skill::kSpinAttack))
		{
			EnableCombo(kSpinAttack);
		}
	}
	//落下攻撃
	else
	{
		if (input_->IsPressButton(XINPUT_GAMEPAD_X) && player_->GetPosition().y != 0.0f)
		{
			EnableCombo(kFallingAttack);
		}
	}
}

void PlayerStateAttack::EnableCombo(const NextState& nextState)
{
	workAttack_.comboNext = true;
	workAttack_.comboNextState = nextState;
}

void PlayerStateAttack::UpdateAnimationAndHandlePhase()
{
	//攻撃パラメーターを取得
	const auto& attackParameters = player_->GetActionFlag(Player::ActionFlag::kAerialAttack) ? player_->GetAerialAttackParameters() : player_->GetGroundAttackParameters();

	//現在のコンボインデックスがコンボの最大数を超えていない場合
	if (workAttack_.comboIndex < attackParameters.comboNum)
	{
		//現在のアニメーション時間を取得
		float animationTime = player_->GetAnimator()->GetIsBlendingCompleted() ? player_->GetAnimator()->GetCurrentAnimationTime() : player_->GetAnimator()->GetNextAnimationTime();

		//現在のフェーズの継続時間をチェックし次のフェーズに移行するか決定
		if (workAttack_.inComboPhase < animationStates_[workAttack_.comboIndex].phases.size() - 1 && animationTime > animationStates_[workAttack_.comboIndex].phases[workAttack_.inComboPhase].duration)
		{
			workAttack_.inComboPhase++;
			player_->ApplyParametersToWeapon("PlayerWeapon", animationStates_[workAttack_.comboIndex].phases[workAttack_.inComboPhase]);
		}

		//現在のフェーズに応じた処理を実行
		HandleComboPhase();
	}
}

void PlayerStateAttack::HandleComboPhase()
{
	//移動が完了した場合アニメーションによる座標のずれを補正
	if (workAttack_.isMovementFinished)
	{
		player_->CorrectAnimationOffset();
	}

	//現在のフェーズに応じた処理
	switch (workAttack_.inComboPhase)
	{
	case kCharge:
		ChargeUpdate();
		break;
	case kAttack:
		AttackUpdate();
		break;
	case kRecovery:
		RecoveryUpdate();
		break;
	}
}

void PlayerStateAttack::ChargeUpdate()
{
	//攻撃パラメーターを取得
	const auto& attackParameters = player_->GetActionFlag(Player::ActionFlag::kAerialAttack) ? player_->GetAerialAttackParameters() : player_->GetGroundAttackParameters();

	//敵の座標を取得
	Vector3 enemyPosition = GameObjectManager::GetInstance()->GetGameObject<Enemy>("Enemy")->GetJointWorldPosition("mixamorig:Hips");

	//プレイヤーの座標を取得
	Vector3 playerPosition = player_->GetJointWorldPosition("mixamorig:Hips");

	//移動終了のフラグが立っていない場合
	if (!workAttack_.isMovementFinished)
	{
		//差分ベクトルを計算
		Vector3 sub = enemyPosition - playerPosition;

		//Y軸方向の差分は無視し、水平面上の方向ベクトルを取得
		sub.y = 0.0f;

		//敵との距離が攻撃距離より大きい場合は移動させる
		if (Mathf::Length(sub) > attackParameters.attackDistance)
		{
			player_->Move(velocity_);
		}
		//敵との距離が攻撃距離より近づいた場合
		else
		{
			//移動終了のフラグを立てる
			workAttack_.isMovementFinished = true;
			//プレイヤーの位置をアニメーション後の位置に補正
			player_->SetIsAnimationCorrectionActive(true);
			player_->SetPosition(player_->GetPosition() + Vector3(player_->GetJointLocalPosition("mixamorig:Hips").x, 0.0f, player_->GetJointLocalPosition("mixamorig:Hips").z));
		}
	}
}

void PlayerStateAttack::AttackUpdate()
{
	//武器を取得
	Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>("PlayerWeapon");
	weapon->SetIsTrailActive(true);

	//アニメーション内でのヒットカウントチェック
	if (workAttack_.hitCount < animationStates_[workAttack_.comboIndex].phases[workAttack_.inComboPhase].attackSettings.hitCount)
	{
		//攻撃パラメータを進める
		workAttack_.attackParameter += GameTimer::GetDeltaTime();;

		//攻撃間隔に応じて攻撃判定を切り替え
		if (workAttack_.attackParameter > animationStates_[workAttack_.comboIndex].phases[workAttack_.inComboPhase].attackSettings.hitInterval)
		{
			workAttack_.attackParameter = 0.0f;
			weapon->SetIsAttack(true);
		}
		else
		{
			weapon->SetIsAttack(false);
		}

		//武器がヒットしたかをチェック
		if (weapon->GetIsHit())
		{
			workAttack_.hitCount++;
		}
	}
	else
	{
		//攻撃状態を無効にする
		weapon->SetIsAttack(false);
	}
}

void PlayerStateAttack::RecoveryUpdate()
{
	//硬直フェーズでは攻撃判定を無効化
	Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>("PlayerWeapon");
	weapon->SetIsTrailActive(false);
	weapon->SetIsAttack(false);
}

void PlayerStateAttack::RotatePlayerTowardsEnemy()
{
	//攻撃パラメーターを取得
	const auto& attackParameters = player_->GetActionFlag(Player::ActionFlag::kAerialAttack) ? player_->GetAerialAttackParameters() : player_->GetGroundAttackParameters();

	//敵の座標を取得
	Vector3 enemyPosition = GameObjectManager::GetInstance()->GetGameObject<Enemy>("Enemy")->GetJointWorldPosition("mixamorig:Hips");

	//プレイヤーの座標を取得
	Vector3 playerPosition = player_->GetJointWorldPosition("mixamorig:Hips");

	//差分ベクトルを計算
	Vector3 sub = enemyPosition - playerPosition;

	//ロックオン中または敵との距離が一定より近かったらボスの方向に回転させる
	if (Mathf::Length(sub) < attackParameters.attackDistance || player_->GetLockon()->ExistTarget())
	{
		//水平方向に回転させるのでY軸成分を0にする
		sub.y = 0.0f;
		//回転処理
		player_->Rotate(Mathf::Normalize(sub));
	}
}

void PlayerStateAttack::UpdateComboState()
{
	//現在のアニメーション時間を取得
	float currentTime = player_->GetAnimator()->GetIsBlendingCompleted() ? player_->GetAnimator()->GetCurrentAnimationTime() : player_->GetAnimator()->GetNextAnimationTime();

	//攻撃の時間を取得
	float attackDuration = GetAttackDuration();

	//現在のアニメーションの時間が攻撃時間を超えたら
	if (currentTime > attackDuration)
	{
		//次のコンボが有効な場合
		if (workAttack_.comboNext)
		{
			HandleComboTransition();
		}
		//アニメーションが終了している場合
		else if (player_->GetAnimator()->GetIsAnimationFinished())
		{
			FinalizeAttackState();
		}
	}
}

float PlayerStateAttack::GetAttackDuration()
{
	for (const auto& phase : animationStates_[workAttack_.comboIndex].phases)
	{
		if (phase.name == "Attack")
		{
			return phase.duration;
		}
	}
	return 0.0f; //デフォルトの値を返す
}

void PlayerStateAttack::HandleComboTransition()
{
	//アニメーションを停止
	player_->GetAnimator()->StopAnimation();

	//プレイヤーの位置をアニメーション後の位置に補正
	CorrectPlayerPosition();

	//遷移先の状態に基づいた処理
	switch (workAttack_.comboNextState)
	{
	case kNormalCombo:
		HandleNormalComboTransition();
		break;
	case kDash:
		player_->ChangeState(new PlayerStateDash());
		break;
	case kLaunchAttack:
		player_->ChangeState(new PlayerStateLaunchAttack());
		break;
	case kSpinAttack:
		player_->ChangeState(new PlayerStateSpinAttack());
		break;
	case kFallingAttack:
		player_->ChangeState(new PlayerStateFallingAttack());
		break;
	case kChargeMagic:
		player_->ChangeState(new PlayerStateChargeMagicAttack());
		break;
	}
}

void PlayerStateAttack::CorrectPlayerPosition()
{
	//プレイヤーの位置をアニメーション後の位置に補正
	player_->SetIsAnimationCorrectionActive(true);
	player_->SetPosition(player_->GetPosition() + Vector3(player_->GetJointLocalPosition("mixamorig:Hips").x, 0.0f, player_->GetJointLocalPosition("mixamorig:Hips").z));
}

void PlayerStateAttack::HandleNormalComboTransition()
{
	//コンボ用ワークをリセット
	ResetAttackWork();

	//アニメーションの再生
	PlayAttackAnimation(player_->GetActionFlag(Player::ActionFlag::kAerialAttack));

	//ロックオン中ではない場合の方向転換処理
	if (!player_->GetLockon()->ExistTarget())
	{
		HandleDirectionalChange();
	}

	//速度を計算
	CalculateVelocity();
}

void PlayerStateAttack::ResetAttackWork()
{
	//攻撃用ワークをリセット
	workAttack_ = {
		.attackParameter = 0.0f,
		.hitCount = 0,
		.comboIndex = workAttack_.comboIndex + 1,
		.inComboPhase = 0,
		.comboNext = false,
		.comboNextState = kNormalCombo,
		.isMovementFinished = false
	};
}

void PlayerStateAttack::HandleDirectionalChange()
{
	Vector3 direction = { input_->GetLeftStickX(), 0.0f, input_->GetLeftStickY() };
	if (Mathf::Length(direction) > player_->GetRootParameters().walkThreshold)
	{
		direction = Mathf::RotateVector(Mathf::Normalize(direction), player_->GetCamera()->quaternion_);
		direction.y = 0.0f;
		player_->Rotate(direction);
	}
}

void PlayerStateAttack::FinalizeAttackState()
{
	//アニメーションを停止
	player_->GetAnimator()->StopAnimation();

	//プレイヤーの位置をアニメーション後の位置に補正
	player_->SetIsAnimationCorrectionActive(true);
	player_->SetPosition(player_->GetPosition() + Vector3(player_->GetJointLocalPosition("mixamorig:Hips").x, 0.0f, player_->GetJointLocalPosition("mixamorig:Hips").z));

	if (player_->GetActionFlag(Player::ActionFlag::kAerialAttack))
	{
		player_->SetActionFlag(Player::ActionFlag::kAerialAttack, false);
		player_->ChangeState(new PlayerStateFalling());
	}
	else
	{
		player_->ChangeState(new PlayerStateRoot());
	}
}