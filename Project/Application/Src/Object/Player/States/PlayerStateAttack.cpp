#include "PlayerStateAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Player/States/PlayerStateFalling.h"
#include "Application/Src/Object/Player/States/PlayerStateDash.h"
#include "Application/Src/Object/Player/States/PlayerStateLaunchAttack.h"
#include "Application/Src/Object/Player/States/PlayerStateSpinAttack.h"
#include "Application/Src/Object/Player/States/PlayerStateStun.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Enemy/Enemy.h"

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
	//衝突相手が武器だった場合
	if (Weapon* weapon = dynamic_cast<Weapon*>(other))
	{
		//アニメーションを停止
		player_->StopAnimation();

		//プレイヤーの位置をアニメーション後の位置に補正
		CorrectPlayerPosition();

		//ダメージを食らった処理を実行
		player_->HandleIncomingDamage(weapon, true);
	}
}

void PlayerStateAttack::PlayAttackAnimation(const bool isAerial)
{
	//アニメーションの名前を設定
	std::string animationName = (isAerial ? "AerialAttack" : "GroundAttack") + std::to_string(workAttack_.comboIndex + 1);
	//アニメーションを再生
	player_->PlayAnimation(animationName, 2.4f, false);
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
		player_->SetNextAnimationTime(animationStates_[0].phases[0].duration);
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
	//打ち上げ攻撃
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) && player_->GetIsCooldownComplete(Skill::kLaunchAttack))
	{
		EnableCombo(kLaunchAttack);
	}
	//回転攻撃
	else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y) && player_->GetIsCooldownComplete(Skill::kSpinAttack))
	{
		EnableCombo(kSpinAttack);
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
		float animationTime = player_->GetIsBlendingCompleted() ? player_->GetCurrentAnimationTime() : player_->GetNextAnimationTime();

		//現在のフェーズの継続時間をチェックし次のフェーズに移行するか決定
		if (workAttack_.inComboPhase < animationStates_[workAttack_.comboIndex].phases.size() - 1 && animationTime > animationStates_[workAttack_.comboIndex].phases[workAttack_.inComboPhase].duration)
		{
			workAttack_.inComboPhase++;
			ApplyParametersToWeapon();
		}

		//現在のフェーズに応じた処理を実行
		HandleComboPhase();
	}
}

void PlayerStateAttack::ApplyParametersToWeapon()
{
	//武器を取得
	Weapon* weapon = GameObjectManager::GetInstance()->GetMutableGameObject<Weapon>("PlayerWeapon");

	//現在のフェーズの設定を取得
	const auto& currentPhase = animationStates_[workAttack_.comboIndex].phases[workAttack_.inComboPhase];

	//ダメージを設定
	weapon->SetDamage(currentPhase.attackSettings.damage);

	//ヒットボックスを設定
	weapon->SetHitbox(currentPhase.hitbox);

	//エフェクトの設定を武器に設定
	weapon->SetEffectSettings(currentPhase.effectSettings);

	//ノックバックの設定を武器に設定
	KnockbackSettings knockbackSettings = currentPhase.knockbackSettings;
	knockbackSettings.knockbackVelocity = Mathf::RotateVector(knockbackSettings.knockbackVelocity, player_->GetDestinationQuaternion());
	knockbackSettings.knockbackAcceleration = Mathf::RotateVector(knockbackSettings.knockbackAcceleration, player_->GetDestinationQuaternion());
	weapon->SetKnockbackSettings(knockbackSettings);
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
	Vector3 enemyPosition = GameObjectManager::GetInstance()->GetMutableGameObject<Enemy>("")->GetHipWorldPosition();

	//プレイヤーの座標を取得
	Vector3 playerPosition = player_->GetHipWorldPosition();

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
			player_->SetPosition(player_->GetPosition() + Vector3(player_->GetHipLocalPosition().x, 0.0f, player_->GetHipLocalPosition().z));
		}
	}
}

void PlayerStateAttack::AttackUpdate()
{
	//武器を取得
	Weapon* weapon = GameObjectManager::GetInstance()->GetMutableGameObject<Weapon>("PlayerWeapon");

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
	Weapon* weapon = GameObjectManager::GetInstance()->GetMutableGameObject<Weapon>("PlayerWeapon");
	weapon->SetIsAttack(false);
}

void PlayerStateAttack::RotatePlayerTowardsEnemy()
{
	//攻撃パラメーターを取得
	const auto& attackParameters = player_->GetActionFlag(Player::ActionFlag::kAerialAttack) ? player_->GetAerialAttackParameters() : player_->GetGroundAttackParameters();

	//敵の座標を取得
	Vector3 enemyPosition = GameObjectManager::GetInstance()->GetMutableGameObject<Enemy>("")->GetHipWorldPosition();

	//プレイヤーの座標を取得
	Vector3 playerPosition = player_->GetHipWorldPosition();

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
	float currentTime = player_->GetIsBlendingCompleted() ? player_->GetCurrentAnimationTime() : player_->GetNextAnimationTime();

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
		else if (player_->GetIsAnimationFinished())
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
	player_->StopAnimation();

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
	}
}

void PlayerStateAttack::CorrectPlayerPosition()
{
	//プレイヤーの位置をアニメーション後の位置に補正
	player_->SetIsAnimationCorrectionActive(true);
	player_->SetPosition(player_->GetPosition() + Vector3(player_->GetHipLocalPosition().x, 0.0f, player_->GetHipLocalPosition().z));
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
	player_->StopAnimation();

	//プレイヤーの位置をアニメーション後の位置に補正
	player_->SetIsAnimationCorrectionActive(true);
	player_->SetPosition(player_->GetPosition() + Vector3(player_->GetHipLocalPosition().x, 0.0f, player_->GetHipLocalPosition().z));

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