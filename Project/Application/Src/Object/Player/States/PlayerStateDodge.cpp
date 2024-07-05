#include "PlayerStateDodge.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/Player.h"
#include "PlayerStateIdle.h"

void PlayerStateDodge::Initialize()
{
	//環境変数の設定
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "DodgeSpeed", dodgeSpeed_);
	//環境変数の適用
	ApplyGlobalVariables();

	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//速度が0でない場合
	if (player_->velocity != Vector3{ 0.0f,0.0f,0.0f })
	{
		//速度ベクトルを計算
		player_->velocity = Mathf::Normalize(player_->velocity) * dodgeSpeed_;

		//アニメーションを設定
		ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
		modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
		modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(1.6f);
		modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);
		modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.001");
	}
	//速度が0の場合
	else
	{
		//BackFlipフラグを立てる
		isBackFlip_ = true;

		//速度ベクトルを計算
		TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
		player_->velocity = Mathf::TransformNormal({ 0.0f,0.0f,-dodgeSpeed_ }, transformComponent->worldTransform_.matWorld_);

		//アニメーションを設定
		ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
		modelComponent->GetModel()->GetAnimation()->SetAnimationTime(0.0f);
		modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(1.6f);
		modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);
		modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.002");
	}
}

void PlayerStateDodge::Update()
{
	//TransformComponentを取得
	TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
	//ModelConponentを取得
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();

	//バックステップ状態でないとき
	if (!isBackFlip_)
	{
		//速度を加算
		transformComponent->worldTransform_.translation_ += player_->velocity * GameTimer::GetDeltaTime();;

		//回避が終わったら
		if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
		{
			//通常状態に戻す
			modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);
			modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(1.0f);
			player_->ChangeState(new PlayerStateIdle());
		}
	}
	else
	{
		//タイマーを進める
		backFlipParameter_ += GameTimer::GetDeltaTime();
		switch (backFlipState_)
		{
		case kAnticipation:
			if (backFlipParameter_ > backFlipAnticipationTime)
			{
				backFlipParameter_ = 0;
				backFlipState_ = kBackFlip;
			}
			break;
		case kBackFlip:
			//速度を加算
			transformComponent->worldTransform_.translation_ += player_->velocity * GameTimer::GetDeltaTime();;
			if (backFlipParameter_ > backFlipTime)
			{
				//通常状態に戻す
				modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);
				modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(1.0f);
				player_->ChangeState(new PlayerStateIdle());
			}
			break;
		case kRecovery:
			break;
		}
	}

	//環境変数の適用
	ApplyGlobalVariables();

	ImGui::Begin("PlayerStateDodge");
	ImGui::DragFloat("BackFlipAnticipationTime", &backFlipAnticipationTime);
	ImGui::DragFloat("BackFlipTime", &backFlipTime);
	ImGui::End();
}

void PlayerStateDodge::Draw(const Camera& camera)
{
}

void PlayerStateDodge::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	dodgeSpeed_ = globalVariables->GetFloatValue(groupName, "DodgeSpeed");
}