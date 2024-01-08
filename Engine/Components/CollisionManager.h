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

	float LenSegOnSeparateAxis(Vector3* Sep, Vector3* e1, Vector3* e2, Vector3* e3);

private:
	std::list<Collider*> colliders_{};
};

