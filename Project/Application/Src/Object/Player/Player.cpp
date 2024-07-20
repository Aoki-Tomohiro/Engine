#include "Player.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Enemy/Enemy.h"

void Player::Initialize()
{
	//GameObjectの初期化
	GameObject::Initialize();

	//Stateの初期化
	ChangeState(new PlayerStateRoot());

	//環境変数の設定
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "WalkSpeed", walkSpeed_);
	globalVariables->AddItem(groupName, "WalkThreshold", walkThreshold_);
	globalVariables->AddItem(groupName, "RunSpeed", runSpeed_);
	globalVariables->AddItem(groupName, "RunThreshold", runThreshold_);
	globalVariables->AddItem(groupName, "DodgeSpeed", dodgeSpeed_);
	globalVariables->AddItem(groupName, "JumpFirstSpeed", jumpFirstSpeed_);
	globalVariables->AddItem(groupName, "GravityAcceleration", gravityAcceleration_);
}

void Player::Update()
{
	//タイトルシーンにいる場合は移動処理をスキップ
	if (!isInTitleScene_)
	{
		//Stateの更新
		state_->Update();
	}

	//回転処理
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.quaternion_ = Mathf::Normalize(Mathf::Slerp(transformComponent->worldTransform_.quaternion_, destinationQuaternion_, 0.4f));

	//ImGuiの処理
	ImGui();

	//デバッグ用の処理
	if (isDebug_)
	{
		ModelComponent* modelComponent = GetComponent<ModelComponent>();
		modelComponent->SetAnimationName(currentAnimationName_);
		modelComponent->GetModel()->GetAnimation()->SetAnimationTime(animationTime_);
	}

	//Gameobjectの更新
	GameObject::Update();

	//環境変数の適用
	ApplyGlobalVariables();
}

void Player::Draw(const Camera& camera)
{
	//Stateの描画
	state_->Draw(camera);

	//GameObjectの描画
	GameObject::Draw(camera);
}

void Player::DrawUI()
{
}

void Player::OnCollision(GameObject* gameObject)
{
	//状態ごとの衝突判定
	state_->OnCollision(gameObject);

	//衝突相手が敵の場合
	if (dynamic_cast<Enemy*>(gameObject))
	{
		//押し戻し処理
		AABBCollider* AABB1 = gameObject->GetComponent<AABBCollider>();
		AABBCollider* AABB2 = GetComponent<AABBCollider>();
		Vector3 overlapAxis = {
			std::min<float>(AABB2->GetWorldCenter().x + AABB2->GetMax().x,AABB1->GetWorldCenter().x + AABB1->GetMax().x) -
			std::max<float>(AABB2->GetWorldCenter().x + AABB2->GetMin().x,AABB1->GetWorldCenter().x + AABB1->GetMin().x),
			std::min<float>(AABB2->GetWorldCenter().y + AABB2->GetMax().y,AABB1->GetWorldCenter().y + AABB1->GetMax().y) -
			std::max<float>(AABB2->GetWorldCenter().y + AABB2->GetMin().y,AABB1->GetWorldCenter().y + AABB1->GetMin().y),
			std::min<float>(AABB2->GetWorldCenter().z + AABB2->GetMax().z,AABB1->GetWorldCenter().z + AABB1->GetMax().z) -
			std::max<float>(AABB2->GetWorldCenter().z + AABB2->GetMin().z,AABB1->GetWorldCenter().z + AABB1->GetMin().z),
		};

		TransformComponent* transform1 = GetComponent<TransformComponent>();
		TransformComponent* transform2 = gameObject->GetComponent<TransformComponent>();
		Vector3 directionAxis{};
		if (overlapAxis.x < overlapAxis.y && overlapAxis.x < overlapAxis.z) {
			//X軸方向で最小の重なりが発生している場合
			directionAxis.x = (transform1->worldTransform_.translation_.x < transform2->worldTransform_.translation_.x) ? -1.0f : 1.0f;
			directionAxis.y = 0.0f;
		}
		else if (overlapAxis.y < overlapAxis.x && overlapAxis.y < overlapAxis.z) {
			//Y軸方向で最小の重なりが発生している場合
			directionAxis.y = (transform1->worldTransform_.translation_.y < transform2->worldTransform_.translation_.y) ? -1.0f : 1.0f;
			directionAxis.x = 0.0f;
		}
		else if (overlapAxis.z < overlapAxis.x && overlapAxis.z < overlapAxis.y)
		{
			directionAxis.z = (transform1->worldTransform_.translation_.z < transform2->worldTransform_.translation_.z) ? -1.0f : 1.0f;
			directionAxis.x = 0.0f;
			directionAxis.y = 0.0f;
		}
		transform1->worldTransform_.translation_ += overlapAxis * directionAxis;
	}
}

void Player::OnCollisionEnter(GameObject* gameObject)
{
	//状態ごとの衝突判定
	state_->OnCollisionEnter(gameObject);
}

void Player::OnCollisionExit(GameObject* gameObject)
{
	//状態ごとの衝突判定
	state_->OnCollisionExit(gameObject);
}

void Player::ChangeState(IPlayerState* state)
{
	state->SetPlayer(this);
	state->Initialize();
	state_.reset(state);
}

void Player::ImGui()
{
	//ImGui
	ImGui::Begin("Player");
	ImGui::DragFloat3("Velocity", &velocity.x);
	ImGui::Checkbox("IsDebug", &isDebug_);
	ImGui::DragFloat("AnimationTime", &animationTime_, 0.001f);
	if (ImGui::BeginCombo("AnimationName", currentAnimationName_.c_str()))
	{
		for (int i = 0; i < animationName_.size(); ++i)
		{
			const bool isSelected = (currentAnimationName_ == animationName_[i]);
			if (ImGui::Selectable(animationName_[i].c_str(), isSelected))
			{
				currentAnimationName_ = animationName_[i];
			}
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::End();
}

void Player::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	walkSpeed_ = globalVariables->GetFloatValue(groupName, "WalkSpeed");
	walkThreshold_ = globalVariables->GetFloatValue(groupName, "WalkThreshold");
	runSpeed_ = globalVariables->GetFloatValue(groupName, "RunSpeed");
	runThreshold_ = globalVariables->GetFloatValue(groupName, "RunThreshold");
	dodgeSpeed_ = globalVariables->GetFloatValue(groupName, "DodgeSpeed");
	jumpFirstSpeed_ = globalVariables->GetFloatValue(groupName, "JumpFirstSpeed");
	gravityAcceleration_ = globalVariables->GetFloatValue(groupName, "GravityAcceleration");
}