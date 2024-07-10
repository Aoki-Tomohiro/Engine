#include "PlayerStateRangedAttack.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Application/Src/Object/Enemy/Enemy.h"

void PlayerStateRangedAttack::Initialize()
{

}

void PlayerStateRangedAttack::Update()
{
}

void PlayerStateRangedAttack::Draw(const Camera& camera)
{
}

void PlayerStateRangedAttack::OnCollision(GameObject* other)
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

void PlayerStateRangedAttack::OnCollisionEnter(GameObject* other)
{
}

void PlayerStateRangedAttack::OnCollisionExit(GameObject* other)
{
}

void PlayerStateRangedAttack::ApplyGlobalVariables()
{
}