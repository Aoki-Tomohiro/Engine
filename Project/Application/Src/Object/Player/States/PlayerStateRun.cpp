#include "PlayerStateRun.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateIdle.h"
#include "Application/Src/Object/Player/States/PlayerStateWalk.h"
#include "Application/Src/Object/Player/States/PlayerStateDodge.h"
#include "Application/Src/Object/Enemy/Enemy.h"

void PlayerStateRun::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();
}

void PlayerStateRun::Update()
{
	//スティックの入力を取得
	Vector3 value = {
		input_->GetLeftStickX(),
		0.0f,
		input_->GetLeftStickY()
	};

	//ロックオン中の場合はアニメーションを切り替える
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	if (player_->lockOn_->ExistTarget())
	{
		//スティックの横入力の絶対値を取得
		float horizontalValue = std::abs(value.x);

		//スティックの縦入力の絶対値を取得
		float verticalValue = std::abs(value.z);

		//横入力値よりも縦入力値のほうが多い場合
		if (verticalValue > horizontalValue)
		{
			//前後の移動がある場合
			if (value.z > 0.0f)
			{
				modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.004"); //前進アニメーション
			}
			else
			{
				modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.005"); //後退アニメーション
			}
		}
		else
		{
			//横の移動がある場合
			if (value.x > 0.0f)
			{
				modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.006"); //右横移動アニメーション
			}
			else
			{
				modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.007"); //左横移動アニメーション
			}
		}
	}
	else
	{
		//前進アニメーション
		modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.004");
	}

	//移動量に速さを反映
	player_->velocity = Mathf::Normalize(value) * player_->runSpeed_;

	//移動ベクトルをカメラの角度だけ回転させる
	Matrix4x4 rotateMatrix = Mathf::MakeRotateYMatrix(player_->camera_->rotation_.y);
	player_->velocity = Mathf::TransformNormal(player_->velocity, rotateMatrix);

	//移動処理
	TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
	transformComponent->worldTransform_.translation_ += player_->velocity * GameTimer::GetDeltaTime();

	//回転処理
	Vector3 rotateVector{};
	if (player_->lockOn_->ExistTarget())
	{
		//差分ベクトルを計算
		TransformComponent* enemyTransformConponent = GameObjectManager::GetInstance()->GetGameObject<Enemy>()->GetComponent<TransformComponent>();
		rotateVector = Mathf::Normalize(enemyTransformConponent->GetWorldPosition() - transformComponent->GetWorldPosition());
		rotateVector.y = 0.0f;
	}
	else
	{
		//速度を正規化
		rotateVector = Mathf::Normalize(player_->velocity);
	}

	//Quaternionの計算
	Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, rotateVector));
	float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, rotateVector);
	player_->destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));

	//スティックの入力値を計算
	float length = Mathf::Length(value);

	//RBで回避状態に遷移
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_SHOULDER))
	{
		player_->ChangeState(new PlayerStateDodge());
	}
	//スティックの入力が走りの閾値をより小さく、歩きの閾値以上の場合、歩き状態に遷移
	else if (length < player_->runThreshold_ && length >= player_->walkThreshold_)
	{
		player_->ChangeState(new PlayerStateWalk());
	}
	//スティックの入力無かった場合、待機状態に遷移
	else if(length < player_->walkThreshold_)
	{
		player_->velocity = { 0.0f,0.0f,0.0f };
		player_->ChangeState(new PlayerStateIdle());
	}
}

void PlayerStateRun::Draw(const Camera& camera)
{
}

void PlayerStateRun::OnCollision(GameObject* other)
{
}

void PlayerStateRun::OnCollisionEnter(GameObject* other)
{
}

void PlayerStateRun::OnCollisionExit(GameObject* other)
{
}