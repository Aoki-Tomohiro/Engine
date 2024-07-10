#include "PlayerStateAirAttack.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Application/Src/Object/Enemy/Enemy.h"

void PlayerStateAirAttack::Initialize()
{

}

void PlayerStateAirAttack::Update()
{
}

void PlayerStateAirAttack::Draw(const Camera& camera)
{
}

void PlayerStateAirAttack::OnCollision(GameObject* other)
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

void PlayerStateAirAttack::OnCollisionEnter(GameObject* other)
{
}

void PlayerStateAirAttack::OnCollisionExit(GameObject* other)
{
}

void PlayerStateAirAttack::ApplyGlobalVariables()
{
}