#pragma once
#include "SphereCollider.h"
#include "AABBCollider.h"
#include "OBBCollider.h"
#include <list>

class CollisionManager
{
public:
	void ClearColliderList();

	void SetColliderList(Collider* collider);

	void CheckAllCollisions();

private:
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

	bool CheckSphereSphereCollision(const SphereCollider* sphere1, const SphereCollider* sphere2);

	bool CheckAABBAABBCollision(const AABBCollider* aabb1, const AABBCollider* aabb2);

	bool CheckOBBOBBCollision(const OBBCollider* obb1, const OBBCollider* obb2);

	bool CheckSphereAABBCollision(const SphereCollider* sphere1, const AABBCollider* aabb1);
	
	bool CheckSphereOBBCollision(const SphereCollider* sphere1, const OBBCollider* obb1);

	bool CheckAABBOBBCollision(const AABBCollider* aabb1, const OBBCollider* obb1);

private:
	std::list<Collider*> colliders_{};
};

