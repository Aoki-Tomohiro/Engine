#include "PlayerStateGroundAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "PlayerStateIdle.h"

//コンボ定数表
const std::array<PlayerStateGroundAttack::ConstGroundAttack, PlayerStateGroundAttack::kComboNum> PlayerStateGroundAttack::kConstAttacks_ =
{
	{
		{ 0.25f, 0.25f, 0.4f, 0.0f, 0.1f,},
		{ 0.3f, 0.3f, 0.4f, 0.0f, 0.1f,},
		{ 0.1f, 0.1f, 0.6f, 0.0f, 0.1f,},
		{ 0.5f, 0.5f, 0.6f, 0.0f, 0.1f,},
	}
};

void PlayerStateGroundAttack::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//武器の初期化
	Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>();
	weapon->SetisParryable(true);

	//アニメーションの初期化
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);
	modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
	modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(3.0f);
	modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.004");

	//環境変数の設定
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	globalVariables->CreateGroup(groupName);
}

void PlayerStateGroundAttack::Update()
{
	//コンボ上限に達していない
	if (workAttack_.comboIndex < kComboNum - 1)
	{
		//攻撃ボタンをトリガーしたら
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
		{
			//コンボ有効
			workAttack_.comboNext = true;
		}
	}
	
	//武器を取得
	Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>();
	//プレイヤーのモデルを取得
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	//アニメーションが終わっていたら通常状態に戻る
	if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
	{
		//コンボ継続なら次のコンボに進む
		if (workAttack_.comboNext)
		{
			//コンボ用パラメータをリセット
			workAttack_.comboNext = false;
			workAttack_.comboIndex++;
			workAttack_.attackParameter = 0.0f;
			parryWindow_ = 0.0f;

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
				Matrix4x4 rotateMatrix = Mathf::MakeRotateYMatrix(player_->camera_->rotation_.y);
				direction = Mathf::TransformNormal(direction, rotateMatrix);

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
			switch (workAttack_.comboIndex)
			{
			case 0:
				modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.004");
				break;
			case 1:
				modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.005");
				break;
			case 2:
				modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.006");
				break;
			case 3:
				modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.007");
				break;
			}
		}
		else
		{
			//武器をリセット
			weapon->SetIsAttack(false);
			weapon->SetisParryable(false);

			//アニメーションをループ再生に戻す
			modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);
			modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(1.0f);

			//通常状態に戻す
			player_->ChangeState(new PlayerStateIdle());

			//これ以降の処理を飛ばす
			return;
		}
	}

	//敵の座標を取得
	Enemy* enemy = GameObjectManager::GetInstance()->GetGameObject<Enemy>();
	TransformComponent* enemyTransformConponent = enemy->GetComponent<TransformComponent>();

	//差分ベクトルを計算
	TransformComponent* playerTransformConponent = player_->GetComponent<TransformComponent>();
	Vector3 sub = enemyTransformConponent->GetWorldPosition() - playerTransformConponent->GetWorldPosition();
	sub.y = 0.0f;

	//距離を計算
	float distance = Mathf::Length(sub);

	//閾値
	float threshold = 10.0f;

	//ボスとの距離が閾値より小さかったらボスの方向に回転させる
	if (distance < threshold || player_->lockOn_->ExistTarget())
	{
		//回転
		Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, Mathf::Normalize(sub)));
		float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, Mathf::Normalize(sub));
		player_->destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
	}

	//アニメーション処理
	if (workAttack_.comboIndex < kComboNum)
	{
		//コンボの合計時間
		float anticipationTime = kConstAttacks_[workAttack_.comboIndex].anticipationTime;
		float chargeTime = anticipationTime + kConstAttacks_[workAttack_.comboIndex].chargeTime;
		float swingTime = chargeTime + kConstAttacks_[workAttack_.comboIndex].swingTime;
		float recoveryTime = swingTime + kConstAttacks_[workAttack_.comboIndex].recoveryTime;

		//攻撃判定の処理
		float currentAnimationTime = modelComponent->GetModel()->GetAnimation()->GetAnimationTime();
		if (currentAnimationTime <= anticipationTime)
		{
		}
		else if (currentAnimationTime > anticipationTime && currentAnimationTime <= chargeTime)
		{
		}
		else if (currentAnimationTime > chargeTime && currentAnimationTime <= swingTime)
		{
			//ヒットタイマーを進める
			workAttack_.attackParameter += GameTimer::GetDeltaTime();
			weapon->SetIsAttack(true);

			//パリィタイマーを進める
			parryWindow_ += GameTimer::GetDeltaTime();
			//タイマーがパリィ成功時間を超えていない場合パリィを有効にする
			if (parryWindow_ < parrySuccessTime_)
			{
				weapon->SetisParryable(true);
			}
			else
			{
				weapon->SetisParryable(false);
			}
		}
		else if (currentAnimationTime > swingTime)
		{
			//当たり判定をなくす
			weapon->SetIsAttack(false);
		}
	}

	//環境変数の適用
	ApplyGlobalVariables();
}

void PlayerStateGroundAttack::Draw(const Camera& camera)
{
}

void PlayerStateGroundAttack::OnCollision(GameObject* other)
{
	Collider* collider = other->GetComponent<Collider>();
	if (collider->GetCollisionAttribute() == kCollisionMaskEnemy)
	{
		Enemy* enemy = dynamic_cast<Enemy*>(other);
		if (enemy->GetIsAttack())
		{
			if (!player_->isInvincible_)
			{
				player_->isInvincible_ = true;
				player_->hp_ -= 10.0f;
			}
		}
	}
}

void PlayerStateGroundAttack::OnCollisionEnter(GameObject* other)
{
}

void PlayerStateGroundAttack::OnCollisionExit(GameObject* other)
{
}

void PlayerStateGroundAttack::ApplyGlobalVariables()
{
}