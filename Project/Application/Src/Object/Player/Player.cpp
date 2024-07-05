#include "Player.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Application/Src/Object/Player/States/PlayerStateIdle.h"
#include "Application/Src/Object/Player/States/PlayerStateGroundAttack.h"
#include "Application/Src/Object/Enemy/Enemy.h"

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

	//Colliderの更新
	AABBCollider* collider = GetComponent<AABBCollider>();
	collider->SetCenter(transformComponent->GetWorldPosition() + colliderOffset_);

	//Gameobjectの更新
	GameObject::Update();

	//ImGui
	ImGui::Begin("Player");
	ImGui::DragFloat3("ColliderOffset", &colliderOffset_.x, 0.1f);
	ImGui::DragFloat("Hp", &hp_);
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
	if (Enemy* enemy = dynamic_cast<Enemy*>(gameObject))
	{
		if (!enemy->GetIsAttack())
		{
			AABBCollider* enemyAABBCollider = enemy->GetComponent<AABBCollider>();
			AABBCollider* playerAABBCollider = GetComponent<AABBCollider>();
			Vector3 overlapAxis = {
				std::min<float>(playerAABBCollider->GetCenter().x + playerAABBCollider->GetMax().x,enemyAABBCollider->GetCenter().x + enemyAABBCollider->GetMax().x) - std::max<float>(playerAABBCollider->GetCenter().x + playerAABBCollider->GetMin().x,enemyAABBCollider->GetCenter().x + enemyAABBCollider->GetMin().x),
				std::min<float>(playerAABBCollider->GetCenter().y + playerAABBCollider->GetMax().y,enemyAABBCollider->GetCenter().y + enemyAABBCollider->GetMax().y) - std::max<float>(playerAABBCollider->GetCenter().y + playerAABBCollider->GetMin().y,enemyAABBCollider->GetCenter().y + enemyAABBCollider->GetMin().y),
				std::min<float>(playerAABBCollider->GetCenter().z + playerAABBCollider->GetMax().z,enemyAABBCollider->GetCenter().z + enemyAABBCollider->GetMax().z) - std::max<float>(playerAABBCollider->GetCenter().z + playerAABBCollider->GetMin().z,enemyAABBCollider->GetCenter().z + enemyAABBCollider->GetMin().z),
			};

			TransformComponent* transformComponent = GetComponent<TransformComponent>();
			TransformComponent* enemyTransformComponent = GetComponent<TransformComponent>();
			Vector3 directionAxis{};
			if (overlapAxis.x < overlapAxis.y && overlapAxis.x < overlapAxis.z) {
				//X軸方向で最小の重なりが発生している場合
				directionAxis.x = (transformComponent->worldTransform_.translation_.x < enemyTransformComponent->worldTransform_.translation_.x) ? -1.0f : 1.0f;
				directionAxis.y = 0.0f;
			}
			else if (overlapAxis.y < overlapAxis.x && overlapAxis.y < overlapAxis.z) {
				//Y軸方向で最小の重なりが発生している場合
				directionAxis.y = (transformComponent->worldTransform_.translation_.y < enemyTransformComponent->worldTransform_.translation_.y) ? -1.0f : 1.0f;
				directionAxis.x = 0.0f;
			}
			else if (overlapAxis.z < overlapAxis.x && overlapAxis.z < overlapAxis.y)
			{
				directionAxis.z = (transformComponent->worldTransform_.translation_.z < enemyTransformComponent->worldTransform_.translation_.z) ? -1.0f : 1.0f;
				directionAxis.x = 0.0f;
				directionAxis.y = 0.0f;
			}

			transformComponent->worldTransform_.translation_ += overlapAxis * directionAxis;
		}
		else
		{
			hp_--;
		}
	}
}

void Player::OnCollisionEnter(GameObject* gameObject)
{

}

void Player::OnCollisionExit(GameObject* gameObject)
{

}

const uint32_t Player::GetComboIndex() const
{
	if (auto state = dynamic_cast<PlayerStateGroundAttack*>(state_.get()))
	{
		return state->GetComboIndex();
	}
	return 0;
}

const uint32_t Player::GetComboNum()
{
	return PlayerStateGroundAttack::kComboNum;
}

void Player::ChangeState(IPlayerState* state)
{
	state->SetPlayer(this);
	state->Initialize();
	state_.reset(state);
}