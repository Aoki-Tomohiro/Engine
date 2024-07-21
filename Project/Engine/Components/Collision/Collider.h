#pragma once
#include "Engine/Components/Component/RenderComponent.h"
#include "Engine/Components/Component/TransformComponent.h"
#include <cstdint>
#include <set>

class Collider : public RenderComponent
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void OnCollision(GameObject* other);

	void OnCollisionEnter(GameObject* other);

	void OnCollisionExit(GameObject* other);

	void SetTransformComponent(TransformComponent* transformComponent) { transformComponent_ = transformComponent; };

	const uint32_t GetCollisionAttribute() const { return collisionAttribute_; };

	void SetCollisionAttribute(uint32_t collisionAttribute) { collisionAttribute_ = collisionAttribute; };

	const uint32_t GetCollisionMask() const { return collisionMask_; };

	void SetCollisionMask(uint32_t collisionMask) { collisionMask_ = collisionMask; };

	void AddCollision(Collider* collider) { currentCollisions_.insert(collider); };

	void RemoveCollision(Collider* collider) { currentCollisions_.erase(collider); };

	const bool IsCollidingWith(Collider* collider) const { return currentCollisions_.find(collider) != currentCollisions_.end(); };

	const bool GetCollisionEnabled() const { return collisionEnabled_; };

	void SetCollisionEnabled(const bool collisionEnabled) { collisionEnabled_ = collisionEnabled; };

	void SetDebugDrawEnabled(const bool debugDrawEnabled) { debugDrawEnabled_ = debugDrawEnabled; };

protected:
	std::set<Collider*> currentCollisions_;

	TransformComponent* transformComponent_ = nullptr;

	uint32_t collisionAttribute_ = 0xffffffff;

	uint32_t collisionMask_ = 0xffffffff;

	bool collisionEnabled_ = true;

	bool debugDrawEnabled_ = false;
};

