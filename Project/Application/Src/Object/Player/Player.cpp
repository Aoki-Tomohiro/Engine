#include "Player.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Application/Src/Object/Player/States/PlayerStateIdle.h"

void Player::Initialize()
{
	//GameObjectの初期化
	GameObject::Initialize();

	//Stateの初期化
	ChangeState(new PlayerStateIdle());
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

	//Gameobjectの更新
	GameObject::Update();

	//Colliderの更新
	AABBCollider* collider = GetComponent<AABBCollider>();
	collider->SetDebugDrawEnabled(true);
	collider->SetCenter(transformComponent->GetWorldPosition() + colliderOffset_);
	collider->SetMin(min_);
	collider->SetMax(max_);

	//ImGui
	ImGui::Begin("Player");
	ImGui::DragFloat3("ColliderOffset", &colliderOffset_.x, 0.1f);
	ImGui::DragFloat3("ColliderMin", &min_.x, 0.1f);
	ImGui::DragFloat3("ColliderMax", &max_.x, 0.1f);
	ImGui::End();
}

void Player::Draw(const Camera& camera)
{
	//Stateの描画
	state_->Draw(camera);

	//GameObjectの描画
	GameObject::Draw(camera);
}

void Player::OnCollision(GameObject* gameObject)
{

}

void Player::OnCollisionEnter(GameObject* gameObject)
{

}

void Player::OnCollisionExit(GameObject* gameObject)
{

}

void Player::ChangeState(IPlayerState* state)
{
	state->SetPlayer(this);
	state->Initialize();
	state_.reset(state);
}