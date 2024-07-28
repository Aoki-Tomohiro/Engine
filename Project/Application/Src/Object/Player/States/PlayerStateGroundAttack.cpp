#include "PlayerStateGroundAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Laser/Laser.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Player/States/PlayerStateKnockback.h"
#include "Application/Src/Object/Player/States/PlayerStateChargedMagicAttack.h"
#include "Application/Src/Object/Player/States/PlayerStateDead.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Enemy/Enemy.h"

//コンボ定数表
const std::array<PlayerStateGroundAttack::ConstGroundAttack, PlayerStateGroundAttack::kComboNum> PlayerStateGroundAttack::kConstAttacks_ =
{
	{
		{0.53f, 0.84f, 1.52f,  11.0f, 0.14f, 1, 10.0f},
		{0.6f,  0.96f, 0.0f,   11.0f, 0.27f, 1, 10.0f},
		{0.54f, 0.74f, 1.333f, 28.0f, 0.08f, 1, 10.0f},
		{1.26f, 1.62f, 2.46f,  22.0f, 0.15f, 1, 10.0f},
	}
};

void PlayerStateGroundAttack::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//武器の初期化
	Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>();
	weapon->SetisParryable(false);

	//アニメーションの初期化
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);
	modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
	modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(2.0f);
	switch (workGroundAttack_.comboIndex)
	{
	case 0:
		modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.014");
		break;
	case 1:
		modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.015");
		break;
	case 2:
		modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.016");
		break;
	case 3:
		modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.017");
		break;
	}

	//ダッシュ攻撃が有効なら
	if (player_->isDashAttack_)
	{
		//攻撃振り時間をなくす
		modelComponent->GetModel()->GetAnimation()->SetAnimationTime(kConstAttacks_[workGroundAttack_.comboIndex].chargeTime);

		//ダッシュ攻撃のフラグをリセット
		player_->isDashAttack_ = false;
	}

	//ダメージを設定
	player_->damage_ = kConstAttacks_[workGroundAttack_.comboIndex].damage;
}

void PlayerStateGroundAttack::Update()
{
	//死亡状態に遷移
	if (player_->hp_ <= 0.0f)
	{
		player_->ChangeState(new PlayerStateDead());
		return;
	}

	//コンボ上限に達していない
	if (workGroundAttack_.comboIndex < kComboNum - 1)
	{
		//攻撃ボタンをトリガーしたら
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
		{
			//コンボ有効
			workGroundAttack_.comboNext = true;
		}
	}

	//武器を取得
	Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>();
	//プレイヤーのモデルを取得
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	//現在のアニメーションの時間を取得
	float currentAnimationTime = modelComponent->GetModel()->GetAnimation()->GetAnimationTime();

	//前のフレームでダメージを食らっていてパリィに成功していなかった場合
	if (wasDamagedLastFrame_)
	{
		if (!weapon->GetIsParrySuccessful())
		{
			Enemy* enemy = GameObjectManager::GetInstance()->GetGameObject<Enemy>();
			player_->hp_ -= enemy->GetDamage();
			player_->isDamaged_ = true;
			//武器をリセット
			weapon->SetIsAttack(false);
			weapon->SetisParryable(false);
			player_->ChangeState(new PlayerStateKnockback());
			return;
		}
		wasDamagedLastFrame_ = false;
	}

	//攻撃が終わっていた場合
	if (currentAnimationTime > kConstAttacks_[workGroundAttack_.comboIndex].swingTime)
	{
		//コンボ継続なら次のコンボに進む
		if (workGroundAttack_.comboNext)
		{
			//コンボ用パラメータをリセット
			workGroundAttack_.comboNext = false;
			workGroundAttack_.comboIndex++;
			workGroundAttack_.attackParameter = 0.0f;
			workGroundAttack_.inComboPhase = 0;
			workGroundAttack_.hitTimer = 0.0f;
			workGroundAttack_.hitCount = 0;
			workGroundAttack_.isMovementFinished = false;
			workGroundAttack_.parryTimer = 0.0f;
			player_->damage_ = kConstAttacks_[workGroundAttack_.comboIndex].damage;

			//コンボ切り替わりの瞬間だけ、スティック入力による方向転換を受け受ける
			const float threshold = 0.7f;

			//回転方向
			Vector3 direction = {
				input_->GetLeftStickX(),
				0.0f,
				input_->GetLeftStickY(),
			};

			//スティックの入力が遊び範囲を超えていたら回転フラグをtrueにする
			if (Mathf::Length(direction) > threshold)
			{
				//回転角度を正規化
				direction = Mathf::Normalize(direction);

				//移動ベクトルをカメラの角度だけ回転する
				direction = Mathf::RotateVector(direction, player_->camera_->quaternion_);
				direction.y = 0.0f;

				//回転処理
				Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, direction));
				float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, direction);
				player_->destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
			}

			//武器の初期化
			weapon->SetIsAttack(false);
			weapon->SetisParryable(false);

			//アニメーションの時間をリセット
			modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
			//次のコンボ用に回転角とアニメーションの初期化
			switch (workGroundAttack_.comboIndex)
			{
			case 0:
				modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.014");
				break;
			case 1:
				modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.015");
				break;
			case 2:
				modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.016");
				break;
			case 3:
				modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.017");
				break;
			}
		}
		else if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
		{
			//武器をリセット
			weapon->SetIsAttack(false);
			weapon->SetisParryable(false);

			//アニメーションをループ再生に戻す
			modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);
			modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(1.0f);

			//通常状態に戻す
			player_->ChangeState(new PlayerStateRoot());

			//これ以降の処理を飛ばす
			return;
		}
		//チャージ魔法状態に遷移
		else if (player_->chargeMagicAttackWork_.isChargeMagicAttack_)
		{
			//武器をリセット
			weapon->SetIsAttack(false);
			weapon->SetisParryable(false);

			//アニメーションをループ再生に戻す
			modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);
			modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(1.0f);

			//チャージ魔法状態に遷移
			player_->ChangeState(new PlayerStateChargedMagicAttack());

			//これ以降の処理を飛ばす
			return;
		}
	}

	//敵の座標を取得
	Enemy* enemy = GameObjectManager::GetInstance()->GetGameObject<Enemy>();
	Vector3 targetPosition = enemy->GetHipWorldPosition();

	//差分ベクトルを計算
	TransformComponent* playerTransformConponent = player_->GetComponent<TransformComponent>();
	Vector3 sub = targetPosition - playerTransformConponent->GetWorldPosition();

	//距離を計算
	float distance = Mathf::Length(sub);

	//ボスとの距離が閾値より小さかったらボスの方向に回転させる
	if (distance < player_->groundAttackParameters_.attackDistance || player_->lockOn_->ExistTarget())
	{
		//回転
		sub.y = 0.0f;
		Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, Mathf::Normalize(sub)));
		float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, Mathf::Normalize(sub));
		player_->destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
	}

	//アニメーション処理
	if (workGroundAttack_.comboIndex < kComboNum)
	{
		//速度を計算
		player_->velocity = { 0.0f,0.0f,kConstAttacks_[workGroundAttack_.comboIndex].moveSpeed };
		player_->velocity = Mathf::TransformNormal(player_->velocity, playerTransformConponent->worldTransform_.matWorld_);

		//チャージ時間を超えていない場合
		if (currentAnimationTime < kConstAttacks_[workGroundAttack_.comboIndex].chargeTime)
		{
			workGroundAttack_.inComboPhase = kCharge;
		}
		//チャージ時間を超えていて、攻撃振り時間を超えていない場合
		else if (currentAnimationTime >= kConstAttacks_[workGroundAttack_.comboIndex].chargeTime && currentAnimationTime < kConstAttacks_[workGroundAttack_.comboIndex].swingTime)
		{
			workGroundAttack_.inComboPhase = kSwing;
		}
		//攻撃振り時間を超えていて、硬直時間を超えていない場合
		else if (currentAnimationTime >= kConstAttacks_[workGroundAttack_.comboIndex].swingTime && currentAnimationTime < kConstAttacks_[workGroundAttack_.comboIndex].recoveryTime)
		{
			workGroundAttack_.inComboPhase = kRecovery;
		}

		//コンボのフェーズごとの処理
		switch (workGroundAttack_.inComboPhase)
		{
		case kCharge:
			//敵と一定距離離れている場合移動させる
			if (distance > player_->groundAttackParameters_.attackDistance)
			{
				if (!workGroundAttack_.isMovementFinished)
				{
					playerTransformConponent->worldTransform_.translation_ += player_->velocity * GameTimer::GetDeltaTime();
				}
			}
			else
			{
				workGroundAttack_.isMovementFinished = true;
			}
			break;
		case kSwing:
			//ヒットタイマーを進める
			workGroundAttack_.hitTimer += modelComponent->GetModel()->GetAnimation()->GetAnimationSpeed() * GameTimer::GetDeltaTime();

			//攻撃判定をつける
			if (workGroundAttack_.hitTimer > kConstAttacks_[workGroundAttack_.comboIndex].attackInterval)
			{
				//ヒット回数が最大数を超えていない場合
				if (workGroundAttack_.hitCount < kConstAttacks_[workGroundAttack_.comboIndex].maxHitCount)
				{
					weapon->SetIsAttack(true);
					workGroundAttack_.hitCount++;
				}
				workGroundAttack_.hitTimer = 0.0f;
			}
			else
			{
				weapon->SetIsAttack(false);
			}

			//パリィタイマーを進める
			workGroundAttack_.parryTimer += GameTimer::GetDeltaTime();

			//タイマーがパリィ成功時間を超えていない場合パリィを有効にする
			if (workGroundAttack_.parryTimer < player_->groundAttackParameters_.parryDuration && !player_->isParrying_)
			{
				//敵からプレイヤーへのベクトルを計算
				sub = Mathf::Normalize(playerTransformConponent->GetWorldPosition() - enemy->GetHipWorldPosition());

				//敵の前方ベクトルを計算
				TransformComponent* enemyTransformComponent = enemy->GetComponent<TransformComponent>();
				Vector3 enemyForwardVector = Mathf::RotateVector({ 0.0f,0.0f,1.0f }, enemyTransformComponent->worldTransform_.quaternion_);

				//外積を計算
				float dot = Mathf::Dot(enemyForwardVector, Mathf::Normalize(sub));

				//敵の正面にいる場合
				if (dot > player_->groundAttackParameters_.frontThreshold)
				{
					weapon->SetisParryable(true);
				}
			}
			else
			{
				weapon->SetisParryable(false);
			}
			break;
		case kRecovery:
			//攻撃判定をなくす
			weapon->SetIsAttack(false);
			break;
		}
	}

	//魔法攻撃の処理
	UpdateEnhancedMagic();
}

void PlayerStateGroundAttack::Draw(const Camera& camera)
{
}

void PlayerStateGroundAttack::OnCollision(GameObject* other)
{
	//パリィ状態でなければ
	if (!player_->isParrying_)
	{
		//衝突相手が敵だった場合
		if (Enemy* enemy = dynamic_cast<Enemy*>(other))
		{
			if (enemy->GetIsAttack())
			{
				wasDamagedLastFrame_ = true;
			}
		}
		//衝突相手がレーザーだった場合
		else if (Laser* laser = dynamic_cast<Laser*>(other))
		{
			wasDamagedLastFrame_ = true;
		}
	}
}

void PlayerStateGroundAttack::OnCollisionEnter(GameObject* other)
{
}

void PlayerStateGroundAttack::OnCollisionExit(GameObject* other)
{
}

void PlayerStateGroundAttack::UpdateEnhancedMagic()
{
	//武器を取得
	Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>();
	//モデルを取得
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();

	//攻撃が当たっていたら
	if (weapon->GetIsHit())
	{
		//強化魔法を出せる状態にする
		workGroundAttack_.isEnhancedMagicAvailable = true;
	}

	//強化魔法を出せる状態の場合
	if (workGroundAttack_.isEnhancedMagicAvailable)
	{
		//入力の受付タイマーを進める
		workGroundAttack_.enhancedMagicInputTimer += GameTimer::GetDeltaTime();
		
		//入力のタイマーが一定値を超えていない時
		if (workGroundAttack_.enhancedMagicInputTimer < player_->magicAttackParameters_.enhancedMagicWindow)
		{
			//Yボタンを押したら
			if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y))
			{
				//強化魔法の成功したフラグを立てる
				workGroundAttack_.isEnhancedMagicSuccess = true;

				//強化魔法を出せない状態にする
				workGroundAttack_.isEnhancedMagicAvailable = false;

				//入力用のタイマーのリセット
				workGroundAttack_.enhancedMagicInputTimer = 0.0f;

				//アニメーションを切り替え
				modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.028");
				modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.82f);
			}
		}
		else
		{
			//強化魔法を出せない状態にする
			workGroundAttack_.isEnhancedMagicAvailable = false;

			//入力用のタイマーのリセット
			workGroundAttack_.enhancedMagicInputTimer = 0.0f;
		}
	}

	//強化魔法を出すことに成功していたら
	if (workGroundAttack_.isEnhancedMagicSuccess)
	{
		//アニメーションの時間が一定値を超えていたら
		if (modelComponent->GetModel()->GetAnimation()->GetAnimationTime() > player_->magicAttackParameters_.enhancedMagicProjectileFireRate)
		{
			//魔法を出す
			workGroundAttack_.isEnhancedMagicSuccess = false;
			player_->AddMagicProjectile(MagicProjectile::MagicType::kEnhanced);

			//プレイヤーの左手のWorldTransformを取得
			WorldTransform worldTransform = modelComponent->GetModel()->GetAnimation()->GetJointWorldTransform("mixamorig:LeftHand");

			//プレイヤーの左手のワールド座標を取得
			Vector3 leftHandWorldPosition = {
				worldTransform.matWorld_.m[3][0],
				worldTransform.matWorld_.m[3][1],
				worldTransform.matWorld_.m[3][2],
			};

			//パーティクルを出す
			for (uint32_t i = 0; i < 360; ++i)
			{
				//速度を決める
				const float kParticleVelocity = 0.3f;
				Quaternion rotate = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,0.0f,1.0f }, i * (3.14f / 180.0f));
				Vector3 velocity = Mathf::RotateVector({ 0.0f,1.0f,0.0f }, player_->destinationQuaternion_ * rotate) * kParticleVelocity;

				//エミッターの生成
				ParticleEmitter* emitter = EmitterBuilder()
					.SetEmitterName("EnhancedMagicSuccess")
					.SetColor({ 1.0f, 0.4f, 0.4f, 1.0f }, { 1.0f, 0.4f, 0.4f, 1.0f })
					.SetEmitterLifeTime(0.1f)
					.SetCount(1)
					.SetFrequency(0.2f)
					.SetLifeTime(0.3f, 0.3f)
					.SetRadius(0.1f)
					.SetScale({ 0.1f,0.1f,0.1f }, { 0.3f,0.3f,0.3f })
					.SetTranslation(leftHandWorldPosition)
					.SetVelocity(velocity, velocity)
					.Build();
				player_->particleSystem_->AddParticleEmitter(emitter);
			}
		}
	}
}