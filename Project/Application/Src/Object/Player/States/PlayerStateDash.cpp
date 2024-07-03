#include "PlayerStateDash.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "PlayerStateIdle.h"
#include "PlayerStateGroundAttack.h"

void PlayerStateDash::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//ロックオン中なら
	if (player_->lockOn_->ExistTarget())
	{
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
	}
	else
	{
		//速度ベクトル
		Vector3 velocity = { 0.0f,0.0f,1.0f };

		//速度ベクトルにプレイヤーの回転を適用
		TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
		velocity = Mathf::TransformNormal(velocity, transformComponent->worldTransform_.matWorld_);

		//速度を計算
		player_->velocity = Mathf::Normalize(velocity) * speed_;
	}

	//RadialBlurをかける
	PostEffects::GetInstance()->GetRadialBlur()->SetIsEnable(true);

	//環境変数の設定
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "DashSpeed", speed_);
	globalVariables->AddItem(groupName, "MaxDashDuration", static_cast<int>(maxDashDuration_));
	globalVariables->AddItem(groupName, "MaxChargeDuration", static_cast<int>(maxChargeDuration_));
	globalVariables->AddItem(groupName, "MaxRecoveryDuration", static_cast<int>(maxRecoveryDuration_));
	globalVariables->AddItem(groupName, "ProximityThreshold", proximityThreshold_);
}

void PlayerStateDash::Update()
{
	//チャージの処理
	if (++currentChargeDuration_ > maxChargeDuration_)
	{
		//チャージ終了フラグを立てる
		isChargingFinished_ = true;
		//プレイヤーを描画しないようにする
		player_->SetIsVisible(false);
	}

	//チャージが終わっていたら
	if (isChargingFinished_)
	{
		//速度を加算
		TransformComponent* playerTransformComponent = player_->GetComponent<TransformComponent>();
		playerTransformComponent->worldTransform_.translation_ += player_->velocity;

		//敵の座標を取得
		Vector3 enemyPosition = GameObjectManager::GetInstance()->GetGameObject<Enemy>()->GetComponent<TransformComponent>()->worldTransform_.translation_;
		enemyPosition.y = playerTransformComponent->worldTransform_.translation_.y;

		//敵に近づいたら速度を0にする
		if (Mathf::Length(enemyPosition - playerTransformComponent->worldTransform_.translation_) < proximityThreshold_)
		{
			player_->velocity = { 0.0f,0.0f,0.0f };
		}

		//ダッシュの時間が終わったら
		if (++currentDashDuration_ > maxDashDuration_)
		{
			//ダッシュ終了フラグを立てる
			isDashFinished_ = true;
			//RadialBlurを切る
			PostEffects::GetInstance()->GetRadialBlur()->SetIsEnable(false);
			//プレイヤーを描画させる
			player_->SetIsVisible(true);
		}
	}

	//ダッシュが終わっていたら
	if (isDashFinished_)
	{
		//硬直時間が終ったら通常状態に戻す
		if (++currentRecoveryDuration_ > maxRecoveryDuration_)
		{
			player_->ChangeState(new PlayerStateIdle());
		}
		//攻撃状態に遷移
		else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
		{
			player_->ChangeState(new PlayerStateGroundAttack());
		}
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
	maxChargeDuration_ = globalVariables->GetIntValue(groupName, "MaxChargeDuration");
	maxRecoveryDuration_ = globalVariables->GetIntValue(groupName, "MaxRecoveryDuration");
	proximityThreshold_ = globalVariables->GetFloatValue(groupName, "ProximityThreshold");
}