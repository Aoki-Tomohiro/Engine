#pragma once
#include "Collider.h"
#include <list>

class CollisionManager
{
public:
	void ClearColliderList();

	void SetColliderList(Collider* collider);

	void CheckAllCollisions();

private:
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

private:
	std::list<Collider*> colliders_{};
};

