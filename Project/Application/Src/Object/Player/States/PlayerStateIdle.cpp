#include "PlayerStateIdle.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "PlayerStateJump.h"
#include "PlayerStateDodge.h"
#include "PlayerStateDash.h"
#include "PlayerStateGroundAttack.h"
#include "PlayerStateRangedAttack.h"
#include "PlayerStateJustDodge.h"

void PlayerStateIdle::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//環境変数の設定
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "MoveSpeed", moveSpeed_);
}

void PlayerStateIdle::Update()
{
	//移動フラグ
	bool isMove = false;

	//閾値
	const float kThreshold = 0.6f;

	//スティックの入力を取得
	player_->velocity = { 
		input_->GetLeftStickX(),
		0.0f,
		input_->GetLeftStickY()
	};

	//速度が閾値を超えていたら移動フラグをtrueにする
	if (Mathf::Length(player_->velocity) > kThreshold)
	{
		isMove = true;
	}

	//移動処理
	if (isMove)
	{
		//移動量に速さを反映
		player_->velocity = Mathf::Normalize(player_->velocity) * moveSpeed_;

		//移動ベクトルをカメラの角度だけ回転させる
		Matrix4x4 rotateMatrix = Mathf::MakeRotateYMatrix(player_->camera_->rotation_.y);
		player_->velocity = Mathf::TransformNormal(player_->velocity, rotateMatrix);

		//移動処理
		TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
		transformComponent->worldTransform_.translation_ += player_->velocity * GameTimer::GetDeltaTime();

		//回転処理
		Vector3 vector = Mathf::Normalize(player_->velocity);
		Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, vector));
		float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, vector);
		player_->destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));

		//走りアニメーションにする
		ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
		modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0");
	}
	else
	{
		//入力がない場合は速度を0にする
		player_->velocity = { 0.0f,0.0f,0.0f };
		//通常のアニメーションにする
		ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
		modelComponent->SetAnimationName("Armature|mixamo.com|Layer0");
	}

	//移動限界座標
	const float kMoveLimitX = 79;
	const float kMoveLimitZ = 79;
	TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
	transformComponent->worldTransform_.translation_.x = std::max<float>(transformComponent->worldTransform_.translation_.x, -kMoveLimitX);
	transformComponent->worldTransform_.translation_.x = std::min<float>(transformComponent->worldTransform_.translation_.x, +kMoveLimitX);
	transformComponent->worldTransform_.translation_.z = std::max<float>(transformComponent->worldTransform_.translation_.z, -kMoveLimitZ);
	transformComponent->worldTransform_.translation_.z = std::min<float>(transformComponent->worldTransform_.translation_.z, +kMoveLimitZ);

	//ジャスト回避の処理
	UpdateJustDodge();

	//ジャンプ状態に変更
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
	{
		player_->ChangeState(new PlayerStateJump());
	}
	//回避状態に変更
	else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_SHOULDER) && !justDodgeSettings_.isJustDodgeSuccess)
	{
		player_->ChangeState(new PlayerStateDodge());
	}
	////ジャスト回避状態に変更
	//else if (justDodgeSettings_.isJustDodgeSuccess)
	//{
	//	player_->ChangeState(new PlayerStateJustDodge());
	//}
	//ダッシュ状態に変更
	else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
	{
		player_->ChangeState(new PlayerStateDash());
	}
	//地上攻撃の状態に遷移
	else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
	{
		player_->ChangeState(new PlayerStateGroundAttack());
	}
	////遠距離攻撃の状態に遷移
	//else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y))
	//{
	//	player_->ChangeState(new PlayerStateRangedAttack());
	//}

	//環境変数の適用
	ApplyGlobalVariables();

	ImGui::Begin("PlayerStateRoot");
	ImGui::End();
}

void PlayerStateIdle::Draw(const Camera& camera)
{
}

void PlayerStateIdle::OnCollision(GameObject* other)
{
	Collider* collider = other->GetComponent<Collider>();
	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemy)
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

void PlayerStateIdle::OnCollisionEnter(GameObject* other)
{
}

void PlayerStateIdle::OnCollisionExit(GameObject* other)
{
}

void PlayerStateIdle::UpdateJustDodge()
{
	//敵の座標を取得
	Enemy* enemy = GameObjectManager::GetInstance()->GetGameObject<Enemy>();
	TransformComponent* enemyTransformComponent = enemy->GetComponent<TransformComponent>();
	//プレイヤーの座標を取得
	TransformComponent* playerTransformComponent = player_->GetComponent<TransformComponent>();

	//敵とプレイヤーの距離を計算
	Vector3 sub = playerTransformComponent->GetWorldPosition() - enemyTransformComponent->GetWorldPosition();

	//敵とプレイヤーの距離がジャスト回避が成功できる距離より近かった場合
	if (Mathf::Length(sub) < justDodgeSettings_.maxJustDodgeDistance)
	{
		//敵の攻撃が終了したらフラグをリセット
		if (!enemy->GetIsAttack())
		{
			justDodgeSettings_.isJustDodgeAvailable = false;
		}

		//ジャスト回避が可能ではない場合
		if (!justDodgeSettings_.isJustDodgeAvailable)
		{
			//敵が攻撃した場合
			if (enemy->GetIsWarning())
			{
				//ジャスト回避が可能にする
				justDodgeSettings_.isJustDodgeAvailable = true;
			}
		}

		//ジャスト回避が可能でジャスト回避が成功していない場合
		if (justDodgeSettings_.isJustDodgeAvailable && !justDodgeSettings_.isJustDodgeSuccess)
		{
			//ジャスト回避のタイマーを進める
			justDodgeSettings_.justDodgeTimer += GameTimer::GetDeltaTime();

			//ジャスト回避のタイマーが一定間隔を超えていない場合
			if (justDodgeSettings_.justDodgeTimer < justDodgeSettings_.justDodgeDuration)
			{
				//RBボタンを押したとき
				if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_SHOULDER))
				{
					//ジャスト回避を成功させる
					justDodgeSettings_.isJustDodgeSuccess = true;
				}
			}
		}
	}
}

void PlayerStateIdle::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	moveSpeed_ = globalVariables->GetFloatValue(groupName, "MoveSpeed");
}