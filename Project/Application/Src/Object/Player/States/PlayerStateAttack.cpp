#include "PlayerStateAttack.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Player/States/PlayerStateFallingAttack.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Weapon/Weapon.h"

void PlayerStateAttack::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//地面にいなければ空中攻撃状態にする
	if (player_->GetPosition().y > 0.0f)
	{
		workAttack_.isAerialAttack = true;
	}

	//アニメーションの設定
	player_->SetIsAnimationLoop(false);
	player_->SetAnimationSpeed(2.8f);
	player_->SetAnimationTime(0.0f);

	if (workAttack_.isAerialAttack)
	{
		//空中コンボアニメーションを設定
		player_->SetAnimationName("Armature.001|mixamo.com|Layer0.019");
		//状態の取得
		for (int32_t i = 0; i < player_->GetAttackParameters().comboNum; ++i)
		{
			states_.push_back(player_->GetAnimationStateManager()->GetAnimationState("AerialAttack" + std::to_string(i + 1)));
		}
	}
	else
	{
		//地上コンボアニメーションを設定
		player_->SetAnimationName("Armature.001|mixamo.com|Layer0.016");
		//状態の取得
		for (int32_t i = 0; i < player_->GetAttackParameters().comboNum; ++i)
		{
			states_.push_back(player_->GetAnimationStateManager()->GetAnimationState("GroundAttack" + std::to_string(i + 1)));
		}
	}

	//最初のフェーズの名前を設定
	phaseName_ = states_[workAttack_.comboIndex].phases[0].name;
}

void PlayerStateAttack::Update()
{
	//次のコンボを有効にする
	EvaluateComboProgress();

	//フェーズの処理
	UpdateAnimationAndHandlePhase();

	//プレイヤーを回転させる
	RotatePlayerTowardsEnemy();

	//アニメーションのずれを修正
	player_->CorrectAnimationOffset();

	//コンボの更新
	UpdateComboState();
}

void PlayerStateAttack::EvaluateComboProgress()
{
	//コンボ上限に達していないとき
	if (workAttack_.comboIndex < player_->GetAttackParameters().comboNum - 1)
	{
		//攻撃ボタンをトリガーしたら
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
		{
			//コンボを有効にする
			workAttack_.comboNext = true;
		}
	}
}

float PlayerStateAttack::GetSwingTime(uint32_t comboIndex)
{
	for (const auto& phase : states_[comboIndex].phases)
	{
		if (phase.name == "Swing" || phase.name == "Swing3")
		{
			return phase.animationTime;
		}
	}
	return 0.0f; //デフォルトの値を返す
}

void PlayerStateAttack::HandleComboTransition()
{
	//攻撃用ワークをリセット
	workAttack_.comboIndex++;
	workAttack_.attackParameter = 0.0f;
	workAttack_.inComboPhase = 0;
	workAttack_.comboNext = false;
	workAttack_.isMovementFinished = false;

	//フェーズの名前を設定
	phaseName_ = states_[workAttack_.comboIndex].phases[0].name;

	//アニメーションの設定
	player_->SetAnimationTime(0.0f);
	if (workAttack_.isAerialAttack)
	{
		switch (workAttack_.comboIndex)
		{
		case 0:
			player_->SetAnimationName("Armature.001|mixamo.com|Layer0.019");
			break;
		case 1:
			player_->SetAnimationTime(0.7f);
			player_->SetAnimationName("Armature.001|mixamo.com|Layer0.020");
			break;
		case 2:
			player_->SetAnimationName("Armature.001|mixamo.com|Layer0.021");
			break;
		}
	}
	else
	{
		switch (workAttack_.comboIndex)
		{
		case 0:
			player_->SetAnimationName("Armature.001|mixamo.com|Layer0.016");
			break;
		case 1:
			player_->SetAnimationName("Armature.001|mixamo.com|Layer0.017");
			break;
		case 2:
			player_->SetAnimationName("Armature.001|mixamo.com|Layer0.018");
			break;
		}
	}

	//ロックオン中ではない場合
	if (!player_->GetLockon()->ExistTarget())
	{
		//コンボの切り替わりの瞬間だけ方向転換を受け付ける
		Vector3 direction = {
			input_->GetLeftStickX(),
			0.0f,
			input_->GetLeftStickY(),
		};

		//スティックの入力が閾値を超えていた場合
		if (Mathf::Length(direction) > player_->GetRootParameters().walkThreshold)
		{
			//カメラの角度だけ回転させる
			direction = Mathf::RotateVector(Mathf::Normalize(direction), player_->GetCamera()->quaternion_);

			//水平方向に回転させるのでY成分を0にする
			direction.y = 0.0f;

			//回転処理
			player_->Rotate(direction);
		}
	}

	//速度を設定
	Quaternion quaternion = player_->GetDestinationQuaternion() * Mathf::Conjugate(Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, std::numbers::pi_v<float>));
	velocity_ = Mathf::RotateVector({ 0.0f,0.0f,player_->GetAttackParameters().moveSpeeds[workAttack_.comboIndex] }, quaternion);
}

void PlayerStateAttack::UpdateComboState()
{
	//現在のアニメーションの時間を取得
	float animationTime = player_->GetAnimationTime();

	//攻撃振り時間を取得
	float swingTime = GetSwingTime(workAttack_.comboIndex);

	//空中攻撃状態の場合
	if (workAttack_.isAerialAttack)
	{
		//地面に足がついたら通常状態に戻す
		if (player_->GetPosition().y <= 0.0f)
		{
			//アニメーション後の座標を代入
			Vector3 hipPosition = player_->GetHipWorldPosition();
			hipPosition.y = 0.0f;
			player_->SetPosition(hipPosition);

			//通常状態に戻す
			player_->ChangeState(new PlayerStateRoot());
			return;
		}
		//落下攻撃状態にする
		else if (input_->IsPressButton(XINPUT_GAMEPAD_X) && input_->IsPressButton(XINPUT_GAMEPAD_A))
		{
			//武器を取得
			Weapon* weapon = GameObjectManager::GetInstance()->GetMutableGameObject<Weapon>("PlayerWeapon");
			weapon->SetIsAttack(false);
			player_->ChangeState(new PlayerStateFallingAttack());
			return;
		}
	}

	//現在のアニメーションの時間が攻撃振りの時間を超えたら
	if (animationTime > swingTime)
	{
		//空中攻撃の場合
		if (workAttack_.isAerialAttack)
		{
			//重力加速度を加算
			velocity_.y += player_->GetAttackParameters().gravityAcceleration;

			//速度を計算
			Vector3 velocity = { 0.0f,velocity_.y,0.0f };

			//移動処理
			player_->Move(velocity);
		}

		//次のコンボが有効な場合
		if (workAttack_.comboNext)
		{
			HandleComboTransition();
		}
		//アニメーションが終了している場合
		else if (player_->GetIsAnimationEnd() && player_->GetPosition().y <= 0.0f)
		{
			//アニメーション後の座標を代入
			Vector3 hipPosition = player_->GetHipWorldPosition();
			hipPosition.y = 0.0f;
			player_->SetPosition(hipPosition);

			//通常状態に戻す
			player_->ChangeState(new PlayerStateRoot());
		}
	}
}

void PlayerStateAttack::UpdateAnimationAndHandlePhase()
{
	if (workAttack_.comboIndex < player_->GetAttackParameters().comboNum)
	{
		//現在のアニメーションの時間を取得
		float animationTime = player_->GetAnimationTime();

		//状態の更新
		for (uint32_t i = 0; i < states_[workAttack_.comboIndex].phases.size() - 1; ++i)
		{
			if (phaseName_ == states_[workAttack_.comboIndex].phases[i].name)
			{
				if (animationTime >= states_[workAttack_.comboIndex].phases[i].animationTime)
				{
					phaseName_ = states_[workAttack_.comboIndex].phases[i + 1].name;
				}
			}
		}

		//コンボのフェーズごとの処理
		HandleComboPhase(workAttack_.inComboPhase);
	}
}

void PlayerStateAttack::RotatePlayerTowardsEnemy()
{
	//敵の座標を取得
	Vector3 enemyPosition = GameObjectManager::GetInstance()->GetConstGameObject<Enemy>("")->GetHipWorldPosition();

	//プレイヤーの座標を取得
	Vector3 playerPosition = player_->GetHipWorldPosition();

	//差分ベクトルを計算
	Vector3 sub = enemyPosition - playerPosition;

	//ロックオン中、または敵との距離が一定より近かったらボスの方向に回転させる
	if (Mathf::Length(sub) < player_->GetAttackParameters().attackDistance || player_->GetLockon()->ExistTarget())
	{
		//水平方向に回転させるのでY軸成分を0にする
		sub.y = 0.0f;

		//回転処理
		player_->Rotate(Mathf::Normalize(sub));
	}
}

void PlayerStateAttack::HandleComboPhase(int phase)
{
	//Chargeフェーズの処理
	if (phaseName_.find("Charge") != std::string::npos)
	{
		ChargeUpdate();
	}
	//Swingフェーズの処理
	else if (phaseName_.find("Swing") != std::string::npos)
	{
		SwingUpdate();
	}
	//Recoveryフェーズの処理
	else if (phaseName_.find("Recovery") != std::string::npos)
	{
		RecoveryUpdate();
	}
}

void PlayerStateAttack::ChargeUpdate()
{
	//敵の座標を取得
	Vector3 enemyPosition = GameObjectManager::GetInstance()->GetConstGameObject<Enemy>("")->GetHipWorldPosition();

	//プレイヤーの座標を取得
	Vector3 playerPosition = player_->GetHipWorldPosition();

	//敵と一定距離離れていて移動が終了していない場合は移動させる
	if (Mathf::Length(enemyPosition - playerPosition) > player_->GetAttackParameters().attackDistance && !workAttack_.isMovementFinished)
	{
		//移動処理
		player_->Move(velocity_);
	}
	//敵に近づいた場合は移動終了フラグを立てる
	else
	{
		workAttack_.isMovementFinished = true;
	}
}

void PlayerStateAttack::SwingUpdate()
{
	//武器を取得
	Weapon* weapon = GameObjectManager::GetInstance()->GetMutableGameObject<Weapon>("PlayerWeapon");

	//攻撃が当たっていたら
	if (weapon->GetIsHit())
	{
		workAttack_.hasHitTargetThisAttack = true;
	}

	//今回の攻撃が当たっていなかった場合
	if (!workAttack_.hasHitTargetThisAttack)
	{
		//武器の判定を付ける
		weapon->SetIsAttack(true);
	}
	//今回の攻撃が当たっていた場合
	else
	{
		//武器の判定をなくす
		weapon->SetIsAttack(false);
	}
}

void PlayerStateAttack::RecoveryUpdate()
{
	//武器の判定をなくす
	Weapon* weapon = GameObjectManager::GetInstance()->GetMutableGameObject<Weapon>("PlayerWeapon");
	weapon->SetIsAttack(false);

	//フラグのリセット
	workAttack_.hasHitTargetThisAttack = false;
}