#include "PlayerStateAirAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Enemy/Enemy.h"

//コンボ定数表
const std::array<PlayerStateAirAttack::ConstAirAttack, PlayerStateAirAttack::kComboNum> PlayerStateAirAttack::kConstAttacks_ =
{
	{
		{0.6f,  0.96f, 2.33f, 6.0f, 0.26f, 1},
		{0.38f, 0.7f,  2.56f, 6.0f, 0.06f, 1},
		{0.7f,  0.88f, 2.4f,  6.0f, 0.13f, 1},
	}
};

void PlayerStateAirAttack::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//空中攻撃のフラグを立てる
	player_->isAirAttack_ = true;

	//武器の初期化
	Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>();
	weapon->SetisParryable(false);

	//アニメーションの初期化
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);
	modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
	modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(2.0f);
	modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.020");

	//ダッシュ攻撃が有効なら
	if (player_->isDashAttack_)
	{
		//攻撃振り時間をなくす
		modelComponent->GetModel()->GetAnimation()->SetAnimationTime(kConstAttacks_[workAirAttack_.comboIndex].chargeTime);

		//ダッシュ攻撃のフラグをリセット
		player_->isDashAttack_ = false;
	}
}

void PlayerStateAirAttack::Update()
{
	//コンボ上限に達していない
	if (workAirAttack_.comboIndex < kComboNum - 1)
	{
		//攻撃ボタンをトリガーしたら
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
		{
			//コンボ有効
			workAirAttack_.comboNext = true;
		}
	}

	//武器を取得
	Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>();
	//プレイヤーのモデルを取得
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	//プレイヤーのトランスフォームを取得
	TransformComponent* playerTransformConponent = player_->GetComponent<TransformComponent>();
	//現在のアニメーションの時間を取得
	float currentAnimationTime = modelComponent->GetModel()->GetAnimation()->GetAnimationTime();

	//攻撃が終わっていた場合
	if (currentAnimationTime > kConstAttacks_[workAirAttack_.comboIndex].swingTime)
	{
		//コンボ継続なら次のコンボに進む
		if (workAirAttack_.comboNext)
		{
			//コンボ用パラメータをリセット
			workAirAttack_.comboNext = false;
			workAirAttack_.comboIndex++;
			workAirAttack_.attackParameter = 0.0f;
			workAirAttack_.inComboPhase = 0;
			workAirAttack_.hitTimer = 0.0f;
			workAirAttack_.hitCount = 0;
			workAirAttack_.isMovementFinished = false;
			workAirAttack_.parryTimer = 0.0f;

			player_->velocity.y = 0.0f;

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
			switch (workAirAttack_.comboIndex)
			{
			case 0:
				modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.020");
				break;
			case 1:
				modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.021");
				break;
			case 2:
				modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.022");
				break;
			}
		}
		else if (playerTransformConponent->worldTransform_.translation_.y <= 0.0f)
		{
			//武器をリセット
			weapon->SetIsAttack(false);
			weapon->SetisParryable(false);

			//アニメーションをループ再生に戻す
			modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);
			modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(1.0f);

			//プレイヤーを地面の座標に設定
			playerTransformConponent->worldTransform_.translation_.y = 0.0f;

			//空中攻撃のフラグの初期化
			player_->isAirAttack_ = false;

			//通常状態に戻す
			player_->ChangeState(new PlayerStateRoot());

			//これ以降の処理を飛ばす
			return;
		}
	}

	//敵の座標を取得
	Enemy* enemy = GameObjectManager::GetInstance()->GetGameObject<Enemy>();
	TransformComponent* enemyTransformConponent = enemy->GetComponent<TransformComponent>();

	//差分ベクトルを計算
	Vector3 sub = enemyTransformConponent->GetWorldPosition() - playerTransformConponent->GetWorldPosition();

	//距離を計算
	float distance = Mathf::Length(sub);

	//ボスとの距離が閾値より小さかったらボスの方向に回転させる
	if (distance < player_->airAttackParameters_.attackDistance || player_->lockOn_->ExistTarget())
	{
		//回転
		sub = Mathf::Normalize(sub);
		sub.y = 0.0f;
		Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, sub));
		float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, sub);
		player_->destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
	}

	//アニメーション処理
	if (workAirAttack_.comboIndex < kComboNum)
	{
		//チャージ時間を超えていない場合
		if (currentAnimationTime < kConstAttacks_[workAirAttack_.comboIndex].chargeTime)
		{
			workAirAttack_.inComboPhase = kCharge;
		}
		//チャージ時間を超えていて、攻撃振り時間を超えていない場合
		else if (currentAnimationTime >= kConstAttacks_[workAirAttack_.comboIndex].chargeTime && currentAnimationTime < kConstAttacks_[workAirAttack_.comboIndex].swingTime)
		{
			workAirAttack_.inComboPhase = kSwing;
		}
		//攻撃振り時間を超えていて、硬直時間を超えていない場合
		else if (currentAnimationTime >= kConstAttacks_[workAirAttack_.comboIndex].swingTime && currentAnimationTime < kConstAttacks_[workAirAttack_.comboIndex].recoveryTime)
		{
			workAirAttack_.inComboPhase = kRecovery;
		}

		//コンボのフェーズごとの処理
		switch (workAirAttack_.inComboPhase)
		{
		case kCharge:
			//敵と一定距離離れている場合移動させる
			if (distance > player_->airAttackParameters_.attackDistance)
			{
				if (!workAirAttack_.isMovementFinished)
				{
					Vector3 horizontalVelocity = { 0.0f,0.0f,kConstAttacks_[workAirAttack_.comboIndex].moveSpeed };
					horizontalVelocity = Mathf::TransformNormal(horizontalVelocity, playerTransformConponent->worldTransform_.matWorld_);
					player_->velocity = { horizontalVelocity.x, player_->velocity.y, horizontalVelocity.z };
				}
			}
			else
			{
				workAirAttack_.isMovementFinished = true;
			}

			//速度に重力加速度を加算
			player_->velocity.y += -player_->airAttackParameters_.attackGravityAcceleration_;

			//速度を加算
			playerTransformConponent->worldTransform_.translation_ += player_->velocity * GameTimer::GetDeltaTime();

			break;
		case kSwing:
			//速度のXとZを0にする
			player_->velocity = { 0.0f,player_->velocity.y,0.0f };

			//プレイヤーの攻撃が当たっていたら少し上に飛ばす
			if (weapon->GetIsHit())
			{
				player_->velocity.y = player_->airAttackParameters_.verticalKnockback;
			}

			//速度に重力加速度を加算
			player_->velocity.y += -player_->airAttackParameters_.attackGravityAcceleration_;

			//速度を加算
			playerTransformConponent->worldTransform_.translation_ += player_->velocity * GameTimer::GetDeltaTime();

			//ヒットタイマーを進める
			workAirAttack_.hitTimer += modelComponent->GetModel()->GetAnimation()->GetAnimationSpeed() * GameTimer::GetDeltaTime();

			//攻撃判定をつける
			if (workAirAttack_.hitTimer > kConstAttacks_[workAirAttack_.comboIndex].attackInterval)
			{
				//ヒット回数が最大数を超えていない場合
				if (workAirAttack_.hitCount < kConstAttacks_[workAirAttack_.comboIndex].maxHitCount)
				{
					weapon->SetIsAttack(true);
					workAirAttack_.hitCount++;
				}
				workAirAttack_.hitTimer = 0.0f;
			}
			else
			{
				weapon->SetIsAttack(false);
			}

			//パリィタイマーを進める
			workAirAttack_.parryTimer += GameTimer::GetDeltaTime();

			//タイマーがパリィ成功時間を超えていない場合パリィを有効にする
			if (workAirAttack_.parryTimer < player_->airAttackParameters_.parryDuration)
			{
				weapon->SetisParryable(true);
			}
			else
			{
				weapon->SetisParryable(false);
			}
			break;
		case kRecovery:
			//速度のXとZを0にする
			player_->velocity = { 0.0f,player_->velocity.y,0.0f };

			//速度に重力加速度を加算
			player_->velocity.y += -player_->airAttackParameters_.gravityAcceleration;

			//速度を加算
			playerTransformConponent->worldTransform_.translation_ += player_->velocity * GameTimer::GetDeltaTime();

			//攻撃判定をなくす
			weapon->SetIsAttack(false);
			break;
		}
	}
}

void PlayerStateAirAttack::Draw(const Camera& camera)
{
}

void PlayerStateAirAttack::OnCollision(GameObject* other)
{
}

void PlayerStateAirAttack::OnCollisionEnter(GameObject* other)
{
}

void PlayerStateAirAttack::OnCollisionExit(GameObject* other)
{
}
