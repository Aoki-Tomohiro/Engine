#include "PlayerStateDash.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "PlayerStateIdle.h"

void PlayerStateDash::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//敵の座標を取得
	Vector3 enemyPosition = GameObjectManager::GetInstance()->GetGameObject<Enemy>()->GetComponent<TransformComponent>()->worldTransform_.translation_;

	//プレイヤーの座標を取得
	Vector3 playerPosition = player_->GetComponent<TransformComponent>()->worldTransform_.translation_;

	//目標の敵のY座標をプレイヤーと同じにする
	enemyPosition.y = playerPosition.y;

	//速度を計算
	player_->velocity = Mathf::Normalize(enemyPosition - playerPosition) * speed_;

	//回転処理
	Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, Mathf::Normalize(player_->velocity)));
	float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, Mathf::Normalize(player_->velocity));
	player_->destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));

	//RadialBlurをかける
	PostEffects::GetInstance()->GetRadialBlur()->SetIsEnable(true);

	//環境変数の設定
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "DashSpeed", speed_);
	globalVariables->AddItem(groupName, "MaxDashDuration", static_cast<int>(maxDashDuration_));
	globalVariables->AddItem(groupName, "ProximityThreshold", proximityThreshold_);
}

void PlayerStateDash::Update()
{
	//速度を加算
	TransformComponent* playerTransformComponent = player_->GetComponent<TransformComponent>();
	playerTransformComponent->worldTransform_.translation_ += player_->velocity;

	//敵の座標を取得
	Vector3 enemyPosition = GameObjectManager::GetInstance()->GetGameObject<Enemy>()->GetComponent<TransformComponent>()->worldTransform_.translation_;
	enemyPosition.y = playerTransformComponent->worldTransform_.translation_.y;

	//ダッシュの時間が終わったら通常状態に戻す
	if (++currentDashDuration_ > maxDashDuration_)
	{
		PostEffects::GetInstance()->GetRadialBlur()->SetIsEnable(false);
		player_->ChangeState(new PlayerStateIdle());
	}
	//敵に近づいたら通常状態に戻す
	else if (Mathf::Length(enemyPosition - playerTransformComponent->worldTransform_.translation_) < proximityThreshold_)
	{
		PostEffects::GetInstance()->GetRadialBlur()->SetIsEnable(false);
		player_->ChangeState(new PlayerStateIdle());
	}

	//環境変数の適用
	ApplyGlobalVariables();

	//ImGui
	ImGui::Begin("PlayerStateDash");
	ImGui::End();
}

void PlayerStateDash::Draw(const Camera& camera)
{
}

void PlayerStateDash::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	speed_ = globalVariables->GetFloatValue(groupName, "DashSpeed");
	maxDashDuration_ = globalVariables->GetIntValue(groupName, "MaxDashDuration");
	proximityThreshold_ = globalVariables->GetFloatValue(groupName, "ProximityThreshold");
}