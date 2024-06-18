#pragma once
#include "Engine/Components/Component/Component.h"
#include "Engine/Components/Component/TransformComponent.h"
#include <cstdint>
#include <set>

class Collider : public Component
{
public:
	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other);

	void OnCollisionEnter(GameObject* other);

	void OnCollisionExit(GameObject* other);

	const uint32_t GetCollisionAttribute() const { return collisionAttribute_; };

	void SetCollisionAttribute(uint32_t collisionAttribute) { collisionAttribute_ = collisionAttribute; };

	const uint32_t GetCollisionMask() const { return collisionMask_; };

	void SetCollisionMask(uint32_t collisionMask) { collisionMask_ = collisionMask; };

	void AddCollision(Collider* collider) { currentCollisions_.insert(collider); };

	void RemoveCollision(Collider* collider) { currentCollisions_.erase(collider); };

	bool IsCollidingWith(Collider* collider) const { return currentCollisions_.find(collider) != currentCollisions_.end(); };

	void SetTransformComponent(TransformComponent* tranformComponent) { transformComponent_ = tranformComponent; };

protected:
	TransformComponent* transformComponent_ = nullptr;

	std::set<Collider*> currentCollisions_;

	uint32_t collisionAttribute_ = 0xffffffff;

	uint32_t collisionMask_ = 0xffffffff;
};

