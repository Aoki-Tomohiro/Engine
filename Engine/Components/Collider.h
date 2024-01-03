#pragma once
#include "Engine/3D/WorldTransform.h"
#include "Engine/Math/AABB.h"

class Collider
{
public:
	virtual void OnCollision(Collider* collider) = 0;

	virtual Vector3 GetWorldPosition() = 0;

	virtual WorldTransform& GetWorldTransform() = 0;

	float GetRadius() { return radius_; };

	void SetRadius(float radius) { radius_ = radius; };

	AABB GetAABB() { return aabb_; };

	void SetAABB(AABB& aabb) { aabb_ = aabb; };

	uint32_t GetCollisionAttribute() { return collisionAttribute_; };

	void SetCollisionAttribute(uint32_t collisionAttribute) { collisionAttribute_ = collisionAttribute; };

	uint32_t GetCollisionMask() { return collisionMask_; };

	void SetCollisionMask(uint32_t collisionMask) { collisionMask_ = collisionMask; };

	uint32_t GetCollisionPrimitive() { return collisionPrimitive_; };

	void SetCollisionPrimitive(uint32_t collisionPrimitive) { collisionPrimitive_ = collisionPrimitive; };

private:
	float radius_ = 1.0f;

	AABB aabb_{ {-1.0f,-1.0f,-1.0f},{1.0f,1.0f,1.0f} };

	uint32_t collisionAttribute_ = 0xffffffff;

	uint32_t collisionMask_ = 0xffffffff;

	uint32_t collisionPrimitive_ = 0b1;
};

