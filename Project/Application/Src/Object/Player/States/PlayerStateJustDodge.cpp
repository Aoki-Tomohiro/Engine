#include "PlayerStateJustDodge.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Illusion/Illusion.h"
#include "PlayerStateIdle.h"

void PlayerStateJustDodge::Initialize()
{
	//Inputのインスタンスを取得
	input_ = Input::GetInstance();
}

void PlayerStateJustDodge::Update()
{
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_SHOULDER))
	{
		player_->ChangeState(new PlayerStateIdle());
	}

	ImGui::Begin("Player");
	ImGui::DragFloat3("Offset", &offset_.x, 0.1f);
	ImGui::End();
}

void PlayerStateJustDodge::Draw(const Camera& camera)
{
}

void PlayerStateJustDodge::OnCollision(GameObject* other)
{
	Collider* collider = other->GetComponent<Collider>();
	if (collider->GetCollisionAttribute() == kCollisionMaskEnemy)
	{
		Enemy* enemy = dynamic_cast<Enemy*>(other);
		if (enemy->GetIsAttack())
		{

		}
	}
}

void PlayerStateJustDodge::OnCollisionEnter(GameObject* other)
{
}

void PlayerStateJustDodge::OnCollisionExit(GameObject* other)
{
}

void PlayerStateJustDodge::ApplyGlobalVariables()
{
}