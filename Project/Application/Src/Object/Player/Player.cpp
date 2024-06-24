#include "Player.h"
#include "Engine/Math/MathFunction.h"

void Player::Initialize()
{
	//GameObjectの初期化
	GameObject::Initialize();

	//Stateの初期化
	ChangeState(new PlayerStateRoot());
}

void Player::Update()
{
	//Stateの更新
	state_->Update();

	//回転処理
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.quaternion_ = Mathf::Normalize(Mathf::Slerp(transformComponent->worldTransform_.quaternion_, destinationQuaternion_, 0.4f));

	//Gameobjectの更新
	GameObject::Update();
}

void Player::Draw(const Camera& camera)
{
	//Stateの描画
	state_->Draw(camera);

	//GameObjectの描画
	GameObject::Draw(camera);
}

void Player::OnCollision(GameObject* collider)
{

}

void Player::OnCollisionEnter(GameObject* collider)
{

}

void Player::OnCollisionExit(GameObject* collider)
{

}

void Player::ChangeState(IPlayerState* state)
{
	state->SetPlayer(this);
	state->Initialize();
	state_.reset(state);
}